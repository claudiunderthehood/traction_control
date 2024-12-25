import random
import numpy as np

import torch
import torch.nn as nn
import torch.optim as optim
from sklearn.preprocessing import MinMaxScaler


def get_device():
    """
    Get the available device (CUDA or CPU).
    """
    return torch.device("cuda" if torch.cuda.is_available() else "cpu")


def save_model_for_cpp(model, input_size, path):
    """
    Save the model as a TorchScript traced model for use in C++ (LibTorch).

    Args:
        model (nn.Module): Trained PyTorch model.
        input_size (int): Number of input features for tracing the model.
        path (str): File path to save the TorchScript model.
    """
    model.eval()
    
    device = next(model.parameters()).device
    example_input = torch.randn(1, input_size).to(device)

    traced_model = torch.jit.trace(model, example_input)

    traced_model.save(path)
    print(f"TorchScript model saved to {path}")



def load_model(model_class, input_size, hidden_size, output_size, path):
    """
    Load a model's state dictionary from a file.

    Args:
        model_class: Class of the model to initialize.
        input_size (int): Number of input features.
        hidden_size (int): Number of neurons in each hidden layer.
        output_size (int): Number of outputs.
        path (str): File path to load the model from.

    Returns:
        nn.Module: The loaded PyTorch model.
    """
    model = model_class(input_size, hidden_size, output_size)
    model.load_state_dict(torch.load(path))
    print(f"Model loaded from {path}")
    return model

def set_seed(seed):
    random.seed(seed)
    np.random.seed(seed)
    torch.manual_seed(seed)
    torch.cuda.manual_seed_all(seed)
    torch.backends.cudnn.deterministic = True
    torch.backends.cudnn.benchmark = False



def train_model_with_early_stopping(
    model, train_loader, val_loader, input_size, output_path,
    criterion=nn.MSELoss(), optimizer=None, scheduler=None, 
    num_epochs=50, patience=5, device='cpu', target_scaler=None
):
    """
    Train the MLP model with early stopping and a learning rate scheduler, and save the best model.

    Args:
        model (nn.Module): The MLP model to train.
        train_loader (DataLoader): DataLoader for training data.
        val_loader (DataLoader): DataLoader for validation data.
        input_size (int): Number of input features for tracing the model for C++.
        output_path (str): Path to save the best model.
        criterion (nn.Module): Loss function (default: MSELoss).
        optimizer (torch.optim.Optimizer): Optimizer (default: Adam).
        scheduler (torch.optim.lr_scheduler): Learning rate scheduler.
        num_epochs (int): Maximum number of epochs to train.
        patience (int): Number of epochs to wait for validation loss improvement.
        device (str): Device to use for training ('cuda' or 'cpu').
        target_scaler (MinMaxScaler): Pre-fitted scaler for targets.

    Returns:
        model (nn.Module): The trained model.
    """
    if target_scaler is None:
        raise ValueError("You must pass a fitted target_scaler.")

    if optimizer is None:
        optimizer = optim.Adam(model.parameters(), lr=1e-3)

    best_val_loss = float('inf')
    patience_counter = 0
    best_model_state = None

    for epoch in range(num_epochs):
        model.train()
        train_loss = 0.0
        for batch_x, batch_y in train_loader:
            batch_x, batch_y = batch_x.to(device), batch_y.to(device)
            optimizer.zero_grad()
            outputs = model(batch_x)
            loss = criterion(outputs, batch_y)
            loss.backward()
            optimizer.step()
            train_loss += loss.item() * batch_x.size(0)

        train_loss /= len(train_loader.dataset)

        model.eval()
        val_loss = 0.0
        with torch.no_grad():
            for batch_x, batch_y in val_loader:
                batch_x, batch_y = batch_x.to(device), batch_y.to(device)
                outputs = model(batch_x)

                outputs_original_scale = target_scaler.inverse_transform(outputs.cpu().numpy())
                batch_y_original_scale = target_scaler.inverse_transform(batch_y.cpu().numpy())

                loss = criterion(torch.tensor(outputs_original_scale), torch.tensor(batch_y_original_scale))
                val_loss += loss.item() * batch_x.size(0)

        val_loss /= len(val_loader.dataset)

        print(f"Epoch {epoch + 1}/{num_epochs}, Train Loss: {train_loss:.4f}, Val Loss: {val_loss:.4f}")

        if scheduler is not None:
            scheduler.step(val_loss)

        if val_loss < best_val_loss:
            best_val_loss = val_loss
            best_model_state = model.state_dict()
            patience_counter = 0
        else:
            patience_counter += 1

        if patience_counter >= patience:
            print("Early stopping triggered.")
            break

    if best_model_state is not None:
        model.load_state_dict(best_model_state)

    save_model_for_cpp(model, input_size, output_path)
    print(f"Best model saved to {output_path}")

    return model