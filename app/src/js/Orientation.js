const abs = Math.abs;
const atan2 = Math.atan2;
const cos = Math.cos;
const sin = Math.sin;
const sqrt = Math.sqrt;
const pow = Math.pow;
const tan = Math.tan;
const max = Math.max;

const toDeg = 180 / Math.PI;
const toRad = Math.PI / 180;

function normalize(v) {
  const mag = sqrt(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);

  if (mag > 0) {
    v[0] /= mag;
    v[1] /= mag;
    v[2] /= mag;
  }

  return v;
}

class Orientation {
  constructor(k = 0.1) {
    this.init = false;
    this.lastTime = 0;
    this.interval = 0;
    this.k = k;

    // normalized acceleration vector
    // coordinates are flipped to match R-ioT coords system
    this.accVector = new Float32Array(3);
    // normalize gyro order and direction according to R-ioT
    this.gyroVector = new Float32Array(3); // third component (yaw) will never be used
    // same as before as a projection vector
    this.gyroEstimate = new Float32Array(3);
    // filtered vector
    this.accEstimate = new Float32Array(3);
    this.prevAccEstimate = new Float32Array(3);

    this.bufferIndex = 0;
    this.maxBufferIndex = 10;
    this.buffer = [ [], [], [] ];
    for (let i = 0; i < 3; i++) {
      for (let j = 0; j < this.maxBufferIndex; j++) {
        this.buffer[i].push(0);
      }
    }
  }

  process(input) {
    const time = Date.now();
    const accEstimate = this.accEstimate;
    const prevAccEstimate = this.prevAccEstimate;
    const gyroEstimate = this.gyroEstimate;

    /**
     * Reorder accelerometer and gyro to conform to R-ioT
     * coordinate system and gyro directions
     */
    const accVector = this.accVector;
    const accOffset = 0;
    accVector[0] = -1 * input[0 + accOffset];
    accVector[1] =  1 * input[1 + accOffset];
    accVector[2] = -1 * input[2 + accOffset];

    const gyroVector = this.gyroVector;
    const gyroOffset = 3;
    gyroVector[0] = -1 * input[2 + gyroOffset];
    gyroVector[1] = -1 * input[1 + gyroOffset];
    gyroVector[2] = -1 * input[0 + gyroOffset];

    normalize(accVector);

    //*
    if (!this.lastTime ||
        isNaN(accEstimate[0]) ||
        isNaN(accEstimate[1]) ||
        isNaN(accEstimate[2])) {
      this.lastTime = time;
      // initialize corrected orientation with normalized accelerometer data
      for (let i = 0; i < 3; i++) {
        accEstimate[i] = accVector[i];
        prevAccEstimate[i] = accEstimate[i];
      }
    } else {
      // define if we use that or use the logical `MotionEvent.interval`
      const dt = time - this.lastTime;

      this.lastTime = time;

      // integrate angle from gyro current values and last result
      // get angles between projection of R on ZX/ZY plane and Z axis, based on last accEstimate

      // gyroVector in deg/s, delta and angle in rad
      const rollDelta = gyroVector[0] * dt * toRad;
      const rollAngle = atan2(accEstimate[0], accEstimate[2]) + rollDelta;

      const pitchDelta = gyroVector[1] * dt * toRad;
      const pitchAngle = atan2(accEstimate[1], accEstimate[2]) + pitchDelta;

      // calculate projection vector from angle Estimates
      gyroEstimate[0] = sin(rollAngle);
      gyroEstimate[0] /= sqrt(1 + pow(cos(rollAngle), 2) * pow(tan(pitchAngle), 2));

      gyroEstimate[1] = sin(pitchAngle);
      gyroEstimate[1] /= sqrt(1 + pow(cos(pitchAngle), 2) * pow(tan(rollAngle), 2));

      // estimate sign of RzGyro by looking in what qudrant the angle Axz is,
      // RzGyro is positive if  Axz in range -90 ..90 => cos(Awz) >= 0
      const signYaw = cos(rollAngle) >= 0 ? 1 : -1;

      // estimate yaw since vector is normalized
      const gyroEstimateSquared = pow(gyroEstimate[0], 2) + pow(gyroEstimate[1], 2);

      gyroEstimate[2] = signYaw * sqrt(max(0, 1 - gyroEstimateSquared));

      // interpolate between estimated values and raw values
      for (let i = 0; i < 3; i++) {
        accEstimate[i] = gyroEstimate[i] * this.k + accVector[i] * (1 - this.k);
      }

      normalize(accEstimate);

      //Rz is too small and because it is used as reference for computing Axz, Ayz
      //it's error fluctuations will amplify leading to bad results. In this case
      //skip the gyro data and just use previous estimate
      if (abs(accEstimate[2]) < 0.1) {
        // use input instead of estimation
        // accVector is already normalized
        for(let i = 0; i < 3; i++) {
          accEstimate[i] = accVector[i];
        }
      }

      const a = 0.3;
      const b = 1 - a;

      for (let i = 0; i < 3; i++) {
        this.buffer[i][this.bufferIndex] = accEstimate[i];
        this.bufferIndex = (this.bufferIndex + 1) % this.maxBufferIndex;

        let sum = 0;
        for (let j = 0; j < this.maxBufferIndex; j++) {
          sum += this.buffer[i][j];
        }
        sum /= this.maxBufferIndex;

        // accEstimate[i] = accEstimate[i] * a + prevAccEstimate[i] * b;
        // prevAccEstimate[i] = accEstimate[i];

        accEstimate[i] = sum;
      }
    }
    //*/

    return [
      accEstimate[0],
      accEstimate[1],
      accEstimate[2]
    ];
  }
};

export default Orientation;
