import * as faceapi from 'face-api.js'; 
import * as React from 'react';
import man from './man.jpg';
import './hidden.css'

export class FaceLandmarks extends React.Component {
  constructor(props) {
    super(props)
    this.img = get(props, 'src', man)
    this.face_id = 'face'
  }

  async findFaceLandmarks() {
    const canvas = this.refs.canvas
    const ctx = canvas.getContext("2d")
    let img = document.getElementById(this.face_id)
    
    const displaySize = { width: img.width, height: img.height }
    faceapi.matchDimensions(canvas, displaySize)

    ctx.drawImage(img, 0, 0)

    let landmarks = await detect(img.id)
    const resizedResults = faceapi.resizeResults(landmarks, displaySize)
    faceapi.draw.drawDetections(canvas, resizedResults)
    faceapi.draw.drawFaceLandmarks(canvas, resizedResults)
  }
  
  async componentDidMount() {
    await loadModels()
    await this.findFaceLandmarks() 
  }

  async updateImage(newImg) {
    let img = document.getElementById(this.face_id)
    img.src = newImg
    await this.findFaceLandmarks()
  }

  render() {
    return(
      <div>
        <canvas ref="canvas" width={0} height={0} />
        <img id={this.face_id} ref="face" src={man} className="hidden" alt='Man face'/>
      </div>
    )
  }
}

async function loadModels()
{
    const MODEL_URL = process.env.PUBLIC_URL + '/models';
    console.log(MODEL_URL)
    await Promise.all([
      faceapi.nets.tinyFaceDetector.loadFromUri(MODEL_URL),
      faceapi.nets.faceLandmark68TinyNet.loadFromUri(MODEL_URL),
    ]).catch(error => {
      console.error(error)
    })
}

export async function detect(img_id)
{
    return faceapi.detectAllFaces(img_id, new faceapi.TinyFaceDetectorOptions())
        .withFaceLandmarks(true)
}

function get(object, key, default_value) {
  var result = object[key];
  return (typeof result !== "undefined") ? result : default_value;
}