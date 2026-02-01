from fastapi import FastAPI, UploadFile, File
from fastapi.middleware.cors import CORSMiddleware
from fastapi.staticfiles import StaticFiles
import lsh_core
import json, os, time
from feature_extractor.feature_extractor import FeatureExtractor
from visualization import generate_scatter_plot

app = FastAPI()
app.add_middleware(CORSMiddleware, allow_origins=["*"], allow_methods=["*"], allow_headers=["*"])

# Mount images
IMAGE_DIR = "data/256_ObjectCategories"
app.mount("/images", StaticFiles(directory=IMAGE_DIR), name="images")

extractor = FeatureExtractor()
db = lsh_core.VectorDB(512, 8, 16)
metadata = {}

if os.path.exists("data/vectors.bin"):
    db.load("data/vectors.bin")
if os.path.exists("data/metadata.json"):
    with open("data/metadata.json", "r") as f: metadata = json.load(f)

@app.get("/visualize")
def get_viz():
    return {"plot_base64": generate_scatter_plot(db.get_all())}

@app.post("/search/compare")
async def compare(file: UploadFile = File(...)):
    content = await file.read()
    with open("query.jpg", "wb") as f: f.write(content)
    vec = extractor.extract("query.jpg").tolist()
    
    def get_results(ids):
        return [{"id": i, "path": metadata.get(str(i), "")} for i in ids]

    t1 = time.time()
    lsh_ids = db.query(vec, 10)
    t_lsh = (time.time() - t1) * 1000

    t2 = time.time()
    exact_ids = db.exact_query(vec, 10)
    t_exact = (time.time() - t2) * 1000

    return {
        "lsh": {"results": get_results(lsh_ids), "time_ms": t_lsh},
        "exact": {"results": get_results(exact_ids), "time_ms": t_exact}
    }
@app.get("/debug/db_size")
def get_db_size():
    all_vectors = db.get_all()
    return {
        "vector_count": len(all_vectors),
        "metadata_count": len(metadata),
        "is_empty": len(all_vectors) == 0
    }