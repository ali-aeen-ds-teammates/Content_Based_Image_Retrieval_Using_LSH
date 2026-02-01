import os
import sys
import json
from tqdm import tqdm

current_dir = os.path.dirname(os.path.abspath(__file__))
parent_dir = os.path.dirname(current_dir)
sys.path.append(parent_dir)

import lsh_core
from feature_extractor.feature_extractor import FeatureExtractor

DATASET_PATH = "data/256_ObjectCategories"
DB_FILE = "data/vectors.bin"
METADATA_FILE = "data/metadata.json"

def run_indexing():
    if not os.path.exists("data"): os.makedirs("data")
    extractor = FeatureExtractor()
    db = lsh_core.VectorDB(512, 8, 16)
    
    image_id = 0
    id_to_path = {} # Map ID to Relative Path

    for root, dirs, files in os.walk(DATASET_PATH):
        category = os.path.basename(root)
        for file in tqdm(files, desc=f"Indexing {category}"):
            if file.lower().endswith(('.jpg', '.jpeg', '.png')):
                img_path = os.path.join(root, file)
                vec = extractor.extract(img_path)
                if vec is not None:
                    db.insert(image_id, vec.tolist())
                    # Save the path relative to 256_ObjectCategories
                    id_to_path[image_id] = f"{category}/{file}"
                    image_id += 1
    
    db.save(DB_FILE)
    with open(METADATA_FILE, "w") as f:
        json.dump(id_to_path, f)
    print(f"Done! Indexed {image_id} images.")