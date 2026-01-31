import os
import sys
from tqdm import tqdm
import lsh_core
from feature_extractor.feature_extractor import FeatureExtractor

# Configuration
DATASET_PATH = "data/caltech101"
DB_FILE = "data/vectors.bin"

def run_indexing():
    extractor = FeatureExtractor()
    db = lsh_core.VectorDB(512, 8, 16)
    
    image_id = 0
    # Walk through Caltech-101 subdirectories
    for root, dirs, files in os.walk(DATASET_PATH):
        for file in tqdm(files, desc=f"Indexing {os.path.basename(root)}"):
            if file.lower().endswith(('.jpg', '.jpeg', '.png')):
                img_path = os.path.join(root, file)
                vec = extractor.extract(img_path)
                if vec is not None:
                    db.insert(image_id, vec.tolist())
                    image_id += 1
    
    db.save(DB_FILE)
    print(f"Indexing complete. Total images: {image_id}")

if __name__ == "__main__":
    run_indexing()