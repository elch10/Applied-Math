import React from 'react';
import './App.css';

import {FaceLandmarks} from './facelandmarks';

function App() {
  return (
    <div className="App">
      <header className="App-header" id="container">
        <FaceLandmarks />
        <p>
          Edit <code>src/App.js</code> and save to reload.
        </p>
      </header>
    </div>
  );
}

export default App;
