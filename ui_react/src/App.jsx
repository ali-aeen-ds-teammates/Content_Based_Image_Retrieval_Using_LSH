import React, { useState, useEffect } from 'react';
import axios from 'axios';
import { Upload, Zap, Search, BarChart3, Image as ImageIcon, Clock, Cpu, Database } from 'lucide-react';

const API_URL = "http://localhost:8000";

function App() {
    const [results, setResults] = useState(null);
    const [viz, setViz] = useState("");
    const [loading, setLoading] = useState(false);
    const [preview, setPreview] = useState(null);

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
        setPreview(URL.createObjectURL(file));
        const formData = new FormData();
        formData.append('file', file);
        setLoading(true);
        try {
            const res = await axios.post(`${API_URL}/search/compare`, formData);
            setResults(res.data);
            fetchViz();
        } catch (err) { console.error(err); }
        finally { setLoading(false); }
    };

    return (
        <div className="min-h-screen relative">
            <div className="scanline"></div>

            {/* TOP NAV */}
            <nav className="border-b border-cyan-500/20 bg-black/80 backdrop-blur-lg sticky top-0 z-50">
                <div className="max-w-7xl mx-auto px-6 py-4 flex justify-between items-center">
                    <div className="flex items-center gap-3">
                        <div className="w-8 h-8 bg-cyan-500 rounded flex items-center justify-center neon-glow">
                            <Database size={18} className="text-black" />
                        </div>
                        <h1 className="text-xl font-black tracking-tighter uppercase">
                            VECTOR<span className="text-cyan-400">DB</span>
                        </h1>
                    </div>
                    <div className="flex items-center gap-4 text-[10px] font-mono text-cyan-600 uppercase tracking-widest">
                        <span className="animate-pulse">●</span> Core Engine Status: LSH_ACTIVE_v26
                    </div>
                </div>
            </nav>

            <main className="max-w-7xl mx-auto px-6 py-10 grid grid-cols-1 lg:grid-cols-12 gap-8">
                {/* LEFT COLUMN */}
                <div className="lg:col-span-4 space-y-8">
                    <section className="glass-card border-cyan-500/20 shadow-[0_0_20px_rgba(0,242,255,0.05)]">
                        <h2 className="text-xs font-bold uppercase tracking-[0.2em] text-cyan-400 mb-6 flex items-center gap-2">
                            <Upload size={14} /> Neural Query Input
                        </h2>
                        <label className="group block relative">
                            <div className="w-full h-56 border-2 border-dashed border-slate-800 rounded-xl flex flex-col items-center justify-center bg-black/20 group-hover:border-cyan-500/50 transition-all overflow-hidden">
                                {preview ? (
                                    <img src={preview} className="w-full h-full object-cover" alt="Preview" />
                                ) : (
                                    <div className="text-center">
                                        <ImageIcon size={32} className="mx-auto text-slate-700 group-hover:text-cyan-500 mb-2" />
                                        <p className="text-[10px] text-slate-500 uppercase">Transmit Image File</p>
                                    </div>
                                )}
                            </div>
                            <input type="file" className="hidden" onChange={handleSearch} />
                        </label>
                        {loading && <div className="mt-4 text-[10px] font-mono text-cyan-500 animate-pulse text-center">ANALYZING VECTOR MANIFOLD...</div>}
                    </section>

                    <section className="glass-card">
                        <h2 className="text-xs font-bold uppercase tracking-[0.2em] text-purple-400 mb-6 flex items-center gap-2">
                            <BarChart3 size={14} /> Manifold Visualization
                        </h2>
                        <div className="aspect-square bg-black/40 border border-slate-800 rounded-xl p-2">
                            {viz ? (
                                <img src={`data:image/png;base64,${viz}`} className="w-full h-full object-contain mix-blend-lighten invert grayscale brightness-125" alt="PCA" />
                            ) : (
                                <div className="h-full flex items-center justify-center text-[10px] text-slate-700 uppercase">Awaiting Matrix Data</div>
                            )}
                        </div>
                    </section>
                </div>

                {/* RIGHT COLUMN */}
                <div className="lg:col-span-8">
                    {!results ? (
                        <div className="h-full min-h-[400px] border border-slate-800/40 rounded-[2rem] bg-black/10 flex flex-col items-center justify-center text-slate-700">
                            <Search size={48} strokeWidth={1} />
                            <p className="mt-4 text-xs font-mono uppercase tracking-[0.3em]">Awaiting Search Protocol</p>
                        </div>
                    ) : (
                        <div className="space-y-12">
                            {/* LSH SECTION */}
                            <div className="animate-in fade-in slide-in-from-bottom-4 duration-500">
                                <div className="flex justify-between items-end mb-6">
                                    <h3 className="text-cyan-400 font-black text-xl uppercase italic tracking-tighter flex items-center gap-2">
                                        <Zap size={20} fill="currentColor" /> LSH Optimized Search
                                    </h3>
                                    <div className="text-right font-mono">
                                        <div className="text-2xl text-white">{results.lsh.time_ms.toFixed(3)}<span className="text-xs text-cyan-600">ms</span></div>
                                    </div>
                                </div>
                                <div className="grid grid-cols-2 sm:grid-cols-5 gap-4">
                                    {results.lsh.results.map((item) => (
                                        <div key={`lsh-${item.id}`} className="bg-slate-900/50 border border-slate-800 p-1 rounded-xl hover:border-cyan-500/50 transition-all group">
                                            <img src={`${API_URL}/images/${item.path}`} className="w-full aspect-square object-cover rounded-lg mb-2" />
                                            <div className="px-1 flex justify-between items-center">
                                                <span className="text-[9px] text-slate-500">#{item.id}</span>
                                                <div className="w-1 h-1 bg-cyan-400 rounded-full shadow-[0_0_5px_cyan]"></div>
                                            </div>
                                        </div>
                                    ))}
                                </div>
                            </div>

                            {/* EXACT SECTION */}
                            <div className="pt-8 border-t border-slate-800/50 opacity-60 hover:opacity-100 transition-opacity">
                                <div className="flex justify-between items-end mb-6">
                                    <h3 className="text-slate-400 font-bold text-sm uppercase tracking-widest flex items-center gap-2">
                                        <Clock size={16} /> Brute-Force Exact
                                    </h3>
                                    <span className="text-sm font-mono text-slate-500">{results.exact.time_ms.toFixed(3)} ms</span>
                                </div>
                                <div className="grid grid-cols-5 gap-3">
                                    {results.exact.results.map((item) => (
                                        <div key={`ex-${item.id}`} className="bg-black/20 border border-slate-900 p-1 rounded-lg">
                                            <img src={`${API_URL}/images/${item.path}`} className="w-full aspect-square object-cover rounded-md opacity-50 grayscale hover:opacity-100 hover:grayscale-0 transition-all" />
                                        </div>
                                    ))}
                                </div>
                            </div>
                        </div>
                    )}
                </div>
            </main>

            <footer className="py-10 border-t border-slate-900 bg-black/40 text-center">
                <p className="text-[9px] font-mono text-slate-600 uppercase tracking-[0.5em]">
                    SBU Final Project // Spring 2026 // Vector Retrieval System
                </p>
            </footer>
        </div>
    );
}

export default App;