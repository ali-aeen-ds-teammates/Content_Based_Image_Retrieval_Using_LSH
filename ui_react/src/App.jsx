import React, { useState, useEffect } from 'react';
import axios from 'axios';

const API_URL = "http://localhost:8000";

function App() {
  const [results, setResults] = useState(null);
  const [viz, setViz] = useState("");
  const [loading, setLoading] = useState(false);

  const fetchViz = async () => {
    try {
      const res = await axios.get(`${API_URL}/visualize`);
      setViz(res.data.plot_base64);
    } catch (e) { console.error("Viz failed", e); }
  };

  useEffect(() => { fetchViz(); }, []);

  const handleSearch = async (e) => {
    const file = e.target.files[0];
    if (!file) return;

    const formData = new FormData();
    formData.append('file', file);
    
    setLoading(true);
    try {
      const res = await axios.post(`${API_URL}/search/compare`, formData);
      setResults(res.data);
      fetchViz();
    } catch (err) {
      alert("Backend Error: Make sure uvicorn is running on port 8000");
    } finally {
      setLoading(false);
    }
  };

  return (
    <div style={{ padding: '40px', maxWidth: '1200px', margin: '0 auto', fontFamily: 'system-ui' }}>
      <h1 style={{ color: '#2d3748' }}>SBU Content-Based Image Retrieval</h1>
      <p>Data Structures & Algorithms Final Project - Fall 2025</p>
      <hr />

      <div style={{ display: 'flex', gap: '40px', marginTop: '20px' }}>
        <div style={{ flex: '1' }}>
          <h2>1. Upload Image</h2>
          <input type="file" onChange={handleSearch} style={{ marginBottom: '20px' }} />
          {loading && <p style={{ color: '#3182ce' }}>Processing Vectors...</p>}
          
          <div style={{ marginTop: '30px', padding: '20px', background: '#f7fafc', borderRadius: '8px' }}>
            <h3>Vector Space Visualization (PCA)</h3>
            {viz ? <img src={`data:image/png;base64,${viz}`} style={{ width: '100%', borderRadius: '4px' }} /> : "Loading space..."}
          </div>
        </div>

        <div style={{ flex: '2' }}>
          <h2>2. Search Results</h2>
          {!results && <p style={{ color: '#a0aec0' }}>Waiting for query...</p>}
          
          {results && (
            <div style={{ display: 'flex', flexDirection: 'column', gap: '30px' }}>
              <div style={{ borderLeft: '4px solid #3182ce', paddingLeft: '15px' }}>
                <h3 style={{ color: '#2b6cb0' }}>LSH Approximate (Sub-linear): {results.lsh.time_ms.toFixed(2)}ms</h3>
                <div style={{ display: 'grid', gridTemplateColumns: 'repeat(auto-fill, minmax(100px, 1fr))', gap: '10px' }}>
                  {results.lsh.results.map(item => (
                    <div key={item.id}>
                      <img src={`${API_URL}/images/${item.path}`} style={{ width: '100px', height: '100px', objectFit: 'cover', borderRadius: '4px' }} />
                      <span style={{ fontSize: '10px' }}>ID: {item.id}</span>
                    </div>
                  ))}
                </div>
              </div>

              <div style={{ borderLeft: '4px solid #38a169', paddingLeft: '15px' }}>
                <h3 style={{ color: '#2f855a' }}>Brute Force Exact (Linear): {results.exact.time_ms.toFixed(2)}ms</h3>
                <div style={{ display: 'grid', gridTemplateColumns: 'repeat(auto-fill, minmax(100px, 1fr))', gap: '10px' }}>
                  {results.exact.results.map(item => (
                    <div key={item.id}>
                      <img src={`${API_URL}/images/${item.path}`} style={{ width: '100px', height: '100px', objectFit: 'cover', borderRadius: '4px' }} />
                      <span style={{ fontSize: '10px' }}>ID: {item.id}</span>
                    </div>
                  ))}
                </div>
              </div>
            </div>
          )}
        </div>
      </div>
    </div>
  );
}

export default App;