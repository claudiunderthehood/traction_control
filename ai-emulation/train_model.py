import torch
import pandas as pd
import torch.optim as optim

from sklearn.model_selection import train_test_split
from sklearn.preprocessing import MinMaxScaler
from torch.utils.data import DataLoader, TensorDataset
from modules.MLPClass import MLPModel
from modules.training_tools import get_device, train_model_with_early_stopping, set_seed

SEED = 42

file_path = "./datasets/simulation_data_cleaned.csv"
df = pd.read_csv(file_path)

features = ['slip_ratio', 'angular_velocity', 'linear_speed', 
            'current_brake_torque', 'current_drive_torque', 
            'speed_to_velocity_ratio', 'excess_drive_torque', 'slip_deviation']
targets = ['desired_drive_torque', 'desired_brake_torque']

set_seed(SEED)

X = df[features].values
y = df[targets].values

scaler = MinMaxScaler()
X = scaler.fit_transform(X)

target_scaler = MinMaxScaler()
y = target_scaler.fit_transform(y)

X_train, X_val, y_train, y_val = train_test_split(X, y, test_size=0.2, random_state=42)

device = get_device()
X_train = torch.tensor(X_train, dtype=torch.float32)
y_train = torch.tensor(y_train, dtype=torch.float32)
X_val = torch.tensor(X_val, dtype=torch.float32)
y_val = torch.tensor(y_val, dtype=torch.float32)

train_dataset = TensorDataset(X_train, y_train)
val_dataset = TensorDataset(X_val, y_val)

train_loader = DataLoader(train_dataset, batch_size=64, shuffle=True)
val_loader = DataLoader(val_dataset, batch_size=64, shuffle=False)

input_size = len(features)
hidden_size = 128
output_size = 2
model = MLPModel(input_size, hidden_size, output_size).to(device)

optimizer = optim.Adam(model.parameters(), lr=0.001)
scheduler = torch.optim.lr_scheduler.ReduceLROnPlateau(
    optimizer, mode='min', factor=0.2, patience=2, verbose=True
)

output_path = "./mlp_model_traced.pt"
trained_model = train_model_with_early_stopping(
    model, train_loader, val_loader, input_size, output_path,
    optimizer=optimizer, scheduler=scheduler, num_epochs=50, patience=5, device=device, target_scaler=target_scaler
)