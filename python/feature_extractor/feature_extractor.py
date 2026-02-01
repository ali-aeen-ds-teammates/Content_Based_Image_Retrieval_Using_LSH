import torch
import torch.nn as nn

from torchvision import models, transforms
import numpy as np
import sys , json

from PIL import Image

class FeatureExtractor:

    def __init__(self , device=None):
        self.device = device if device else ('cuda' if torch.cuda.is_available() else 'cpu')


        weights = models.ResNet18_Weights.IMAGENET1K_V1
        self.model = models.resnet18(weights=weights)
        
        self.modules = list(self.model.children())[:-1]
        self.model = nn.Sequential(*self.modules)
        
        self.model.to(self.device)
        self.model.eval()
        
        self.preprocess = transforms.Compose([
            transforms.Resize(256),             # Resize shorter side to 256
            transforms.CenterCrop(224),         # Crop center 224x224
            transforms.ToTensor(),              # Convert to Tensor [0, 1]
            transforms.Normalize(               # Normalize using ImageNet mean/std
                mean=[0.485, 0.456, 0.406],
                std=[0.229, 0.224, 0.225]
            )
        ])
        
        
        
    def extract(self, image_path:str) -> np.ndarray: 
        try: 
            image = Image.open(image_path).convert('RGB')

            input_tensor = self.preprocess(image)
            input_batch = input_tensor.unsqueeze(0)
            input_batch = input_batch.to(self.device)

            with torch.no_grad(): 
                output = self.model(input_batch)

            feature_vector = output.cpu().numpy().flatten()
            return feature_vector
    
        except Exception as e:
            sys.stderr.write(f"Error processing {image_path}: {str(e)}\n")
            return None
        
    def output_json(self , path): 
        vector = self.extract(path)
        j = json.dumps(vector.tolist())
        return j  
        
    def output_numpy(self , image_path , out_path): 
        vector = self.extract(image_path)
        np.save(out_path, vector)