import matplotlib.pyplot as plt
from sklearn.decomposition import PCA
import numpy as np
import io
import base64

def generate_scatter_plot(vector_map):
    if not vector_map:
        return ""
    
    ids = list(vector_map.keys())
    data = np.array(list(vector_map.values()))
    
    # Reduce 512D to 2D for visualization
    pca = PCA(n_components=2)
    reduced_data = pca.fit_transform(data)
    
    plt.figure(figsize=(8, 6))
    plt.scatter(reduced_data[:, 0], reduced_data[:, 1], alpha=0.5)
    
    # Convert plot to base64 string for easy display in GUI
    buf = io.BytesIO()
    plt.savefig(buf, format='png')
    buf.seek(0)
    img_str = base64.b64encode(buf.read()).decode('utf-8')
    plt.close()
    return img_str