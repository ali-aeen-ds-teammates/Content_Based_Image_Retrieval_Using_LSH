import React, { useState, useEffect } from 'react';
import axios from 'axios';

const API_URL = "http://localhost:8000";

function App() {
  const [results, setResults] = useState(null);
  const [viz, setViz] = useState("");
  const [loading, setLoading] = useState(false);

  const fetchViz = async () => {
    const res = await axios.get(`${API_URL}/visualize`);
    setViz(res.data.plot_base64);
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
      fetchViz(); // Update plot in case data changed
    } catch (err) {
      alert("Backend not running or Error processing image");
    } finally {
      setLoading(false);
    }
  };

  return (
    <div style={{ padding: '20px', fontFamily: 'sans-serif', backgroundColor: '#f0f2f5', minHeight: '100vh' }}>
      <header style={{ borderBottom: '2px solid #ddd', marginBottom: '20px' }}>
        <h1>SBU Content-Based Image Retrieval</h1>
        <p>LSH Indexing vs Brute Force Search</p>
      </header>

      <div style={{ display: 'flex', gap: '20px' }}>
        <div style={{ flex: 1, background: 'white', padding: '20px', borderRadius: '8px shadow' }}>
          <h3>1. Query Image</h3>
          <input type="file" onChange={handleSearch} />
          {loading && <p style={{ color: 'blue' }}>Analyzing Image...</p>}

          <div style={{ marginTop: '40px' }}>
            <h3>3. Vector Space Visualization</h3>
            {viz ? <img src={`data:image/png;base64,${viz}`} style={{ width: '100%' }} /> : "Loading Plot..."}
          </div>
        </div>

        <div style={{ flex: 1, background: 'white', padding: '20px', borderRadius: '8px' }}>
          <h3>2. Search Results</h3>
          {results ? (
            <div style={{ display: 'flex', flexDirection: 'column', gap: '20px' }}>
              <div style={{ border: '1px solid #90cdf4', padding: '10px', borderRadius: '5px' }}>
                <h4 style={{ color: '#2b6cb0' }}>LSH (Approximate): {results.lsh.time_ms.toFixed(2)}ms</h4>
                <div style={{ display: 'flex', flexWrap: 'wrap', gap: '10px' }}>
                  {results.lsh.results.map(id => <div key={id} style={{ background: '#ebf8ff', padding: '10px' }}>ID: {id}</div>)}
                </div>
              </div>

              <div style={{ border: '1px solid #9ae6b4', padding: '10px', borderRadius: '5px' }}>
                <h4 style={{ color: '#2f855a' }}>Brute Force (Exact): {results.exact.time_ms.toFixed(2)}ms</h4>
                <div style={{ display: 'flex', flexWrap: 'wrap', gap: '10px' }}>
                  {results.exact.results.map(id => <div key={id} style={{ background: '#f0fff4', padding: '10px' }}>ID: {id}</div>)}
                </div>
              </div>
            </div>
          ) : <p>Upload an image to start search.</p>}
        </div>
      </div>
    </div>
  );
}

export default App;