<template>
  <div id="app">
    <div id="tabs">
      <router-link to="/control" id="control-tab" class="tab">
        Control
      </router-link>
      <router-link to="/settings" id="settings-tab" class="tab">
        <img id="settings-icon">
      </router-link>
    </div>

    <router-view></router-view>
  </div>
</template>

<script>
import router from '../js/router';
import Orientation from '../js/Orientation';

export default {
  router,
  components: {},
  props: [ 'childReady', 'inputPort' ],
  watch: {
    childReady: {
      immediate: true,
      deep: true,
      handler: function(val) {
        if (val) this.init();
      },
    },
    inputPort: {
      handler: function(val) {
        this.startListeningOSC(val);
      }
    }
  },
  methods: {
    init() {
      // window.osc.startListening(this.$store.state.oscConfig.inputPort, () => {
      //   console.log('...  now listening');
      // }, (err) => console.error(err));
      // this.startListeningOSC(this.$store.state.oscConfig.inputPort);

      // faking accelerometer for tests in emulator
      // setInterval(() => {
      //   const amp = 3;
      //   const offset = -1.5;
      //   this.$store.commit('updateMotionValues', {
      //     x: Math.random() * amp + offset,
      //     y: Math.random() * amp + offset,
      //     z: Math.random() * amp + offset,
      //     alpha: Math.random * 180,
      //     beta: Math.random * 180,
      //     gamma: Math.random * 180,
      //   });
      // }, 50);

      //*
      this.$store.watch(this.$store.getters.playing, (val, oldVal) => {
        if (val !== oldVal) {
          if (val) {
            this.startListeningDeviceMotion();
            this.startListeningDeviceOrientation();
            this.startSendingOSC();
          } else {
            this.stopSendingOSC();
            this.stopListeningDeviceMotion();
            this.stopListeningDeviceOrientation();
          }
        }
      });
      //*/

      // this.startListeningDeviceMotion();
      // this.startListeningDeviceOrientation();
      // this.startSendingOSC();
      // this.startListeningOSC(this.$store.state.oscConfig.inputPort); // really needed ?
    },
    startListeningDeviceMotion() {
      window.addEventListener('devicemotion', this.onDeviceMotion, true);
    },
    stopListeningDeviceMotion() {
      window.removeEventListener('devicemotion', this.onDeviceMotion);
    },
    onDeviceMotion(e) {
      const values = {};

      if (e.accelerationIncludingGravity &&
          e.accelerationIncludingGravity.x &&
          e.accelerationIncludingGravity.y &&
          e.accelerationIncludingGravity.z) {

        values.x = e.accelerationIncludingGravity.x;
        values.y = e.accelerationIncludingGravity.y;
        values.z = e.accelerationIncludingGravity.z;

        if (e.rotationRate &&
            e.rotationRate.alpha &&
            e.rotationRate.beta &&
            e.rotationRate.gamma) {

          values.alpha = e.rotationRate.alpha;
          values.beta = e.rotationRate.beta;
          values.gamma = e.rotationRate.gamma;
        }

        // we commit even if the phone doesn't have a gyroscope
        // (angular velocities will remain equal to zero)
        this.$store.commit('updateMotionValues', values);
      }

      // this.sendOSC('/accel', [ values.accx, values.accy, values.accz ]);
    },
    startListeningDeviceOrientation() {
      window.addEventListener('deviceorientation', this.onDeviceOrientation, true);
    },
    stopListeningDeviceOrientation() {
      window.removeEventListener('deviceorientation', this.onDeviceOrientation);
    },
    onDeviceOrientation(e) {
      const values = {};

      if (e.alpha && e.beta && e.gamma) {
        values.alpha = e.alpha;
        values.beta = e.beta;
        values.gamma = e.gamma;

        this.$store.commit('updateOrientationValues', values);
      }
    },
    startSendingOSC(interval = 10) { // interval between consecutive frames in ms
      this.sendOscId = setInterval(() => {
        this.sendOSC('/softmirror', [
          this.$store.state.oscConfig.deviceIdentifier,
          this.$store.state.motionValues.x,
          this.$store.state.motionValues.y,
          this.$store.state.motionValues.z,
          this.$store.state.motionValues.alpha,
          this.$store.state.motionValues.beta,
          this.$store.state.motionValues.gamma,
          this.$store.state.orientationValues.alpha,
          this.$store.state.orientationValues.beta,
          this.$store.state.orientationValues.gamma,
        ]);
      }, interval);
    },
    stopSendingOSC() {
      clearInterval(this.sendOscId);
      this.sendOscId = null;
    },
    startListeningOSC(inputPort) {
      console.log('starting listening OSC messages on port ' + inputPort + ' ...');
      window.osc.startListening(inputPort, () => {
        console.log('...  now listening');
        // window.osc.on('/vibro/now', this._onVibroNow);
        // window.osc.on('/vibro/pulse', this._onVibroPulse)
      }, (err) => console.error(err));
    },
    stopListeningOSC() {
      return new Promise((resolve, reject) => {
        window.osc.stopListening(() => {
          // window.osc.removeListener('/vibro/now', this._onVibroNow)
          // window.osc.removeListener('/vibro/pulse', this._onVibroPulse)
          resolve();
        }, () => {
          reject();
        });
      });
    },
    sendOSC(address, args) {
      window.osc.send({
        remoteAddress: this.$store.state.oscConfig.hostIP,
        remotePort: this.$store.state.oscConfig.outputPort,
        address: address,
        arguments: args,
      });
    },
  },
};
</script>