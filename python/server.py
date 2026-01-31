from fastapi import FastAPI, UploadFile, File
from fastapi.middleware.cors import CORSMiddleware
from visualization import generate_scatter_plot
import lsh_core
from feature_extractor.feature_extractor import FeatureExtractor
import time
import os

app = FastAPI()

# Lead Architect Fix: Enable CORS for React development
app.add_middleware(
    CORSMiddleware,
    allow_origins=["*"], 
    allow_methods=["*"], 
    allow_headers=["*"],
)

extractor = FeatureExtractor()
db = lsh_core.VectorDB(512, 8, 16)
DB_PATH = "data/vectors.bin"

if os.path.exists(DB_PATH):
    db.load(DB_PATH)

@app.get("/visualize")
def get_viz():
    all_vecs = db.get_all()
    return {"plot_base64": generate_scatter_plot(all_vecs)}

@app.post("/search/compare")
async def compare_search(file: UploadFile = File(...)):
    content = await file.read()
    temp_filename = f"query_{int(time.time())}.jpg"
    with open(temp_filename, "wb") as f: f.write(content)
    
    try:
        vec = extractor.extract(temp_filename).tolist()
        
        t1 = time.time()
        lsh_res = db.query(vec, 10)
        t_lsh = (time.time() - t1) * 1000
        
        t2 = time.time()
        exact_res = db.exact_query(vec, 10)
        t_exact = (time.time() - t2) * 1000
        
        return {
            "lsh": {"results": lsh_res, "time_ms": t_lsh},
            "exact": {"results": exact_res, "time_ms": t_exact}
        }
    finally:
        if os.path.exists(temp_filename): os.remove(temp_filename)