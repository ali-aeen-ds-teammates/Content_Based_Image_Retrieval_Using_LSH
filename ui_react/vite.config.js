import { defineConfig } from 'vite';

export default defineConfig({
  plugins: [
    {
      name: 'strip-broken-sourcemaps',
      transform(code, id) {
        // This catches references to the broken .map files in lucide-react
        // and removes them before they trigger the Vite/Esbuild error.
        if (id.includes('node_modules/lucide-react')) {
          return {
            code: code.replace(/\/\/# sourceMappingURL=.*/g, ''),
            map: null
          };
        }
      }
    }
  ],
  server: {
    fs: {
      strict: false,
    },
  },
  build: {
    sourcemap: false,
  }
});