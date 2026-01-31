from fastapi import FastAPI, UploadFile, File
from visualization import generate_scatter_plot
import lsh_core
from feature_extractor.feature_extractor import FeatureExtractor
import time

app = FastAPI()
extractor = FeatureExtractor()
db = lsh_core.VectorDB(512, 8, 16)

@app.get("/visualize")
def get_viz():
    all_vecs = db.get_all()
    return {"plot_base64": generate_scatter_plot(all_vecs)}

@app.post("/search/compare")
async def compare_search(file: UploadFile = File(...)):
    content = await file.read()
    with open("query.jpg", "wb") as f: f.write(content)
    vec = extractor.extract("query.jpg").tolist()
    
    t1 = time.time()
    lsh_res = db.query(vec, 5)
    t_lsh = (time.time() - t1) * 1000
    
    t2 = time.time()
    exact_res = db.exact_query(vec, 5)
    t_exact = (time.time() - t2) * 1000
    
    return {
        "lsh": {"results": lsh_res, "time_ms": t_lsh},
        "exact": {"results": exact_res, "time_ms": t_exact}
    }