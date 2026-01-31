import time
import numpy as np
import pandas as pd
import lsh_core

def run_benchmark(db, num_queries=50):
    all_vecs = list(db.get_all().values())
    if not all_vecs: return
    
    results = []
    for i in range(num_queries):
        query_vec = all_vecs[np.random.randint(0, len(all_vecs))]
        
        # Exact Search
        start = time.time()
        db.exact_query(query_vec, 10)
        t_exact = (time.time() - start) * 1000
        
        # LSH Search
        start = time.time()
        db.query(query_vec, 10)
        t_lsh = (time.time() - start) * 1000
        
        results.append({"query": i, "exact_ms": t_exact, "lsh_ms": t_lsh})
    
    df = pd.DataFrame(results)
    df.to_excel("performance_report.xlsx", index=False)
    print("Benchmark results saved to performance_report.xlsx")