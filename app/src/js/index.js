import Vue from 'vue/dist/vue.js';
import App from '../components/App.vue';
import VueRouter from 'vue-router';
import store from './store';

Vue.use(VueRouter);

// entry point found here :
// https://www.raymondcamden.com/2017/12/06/quick-example-of-apache-cordova-and-vuejs

const main = new Vue({
  el: '#main',
  store,
  components: {
    'main-component': App
  },
  data: {
    parentReady: false,
    parentInputPort: 8000,
  },
  methods: {
    init: function() {
      this.$store.watch(this.$store.getters.inputPort, (val, oldVal) => {
        if (val !== oldVal) {
          console.log('new input port ' + val);
          delete window.osc;
          window.osc = new OSC();
          this.parentInputPort = val;
        }
      });

      // quick n' dirty way to make OSC plugin accessible to other components
      // needs to be reinstanciated when input port has to change (see above)
      window.osc = new OSC();

      // let's do this now
      // https://cordova.apache.org/docs/en/latest/reference/cordova-plugin-screen-orientation/
      window.screen.orientation.lock('portrait-primary');

      // load settings from persistent file or create defaut if !exists (see store.js)
      store.dispatch('retrieve').then(() => {
        this.parentInputPort = this.$store.state.oscConfig.inputPort;
      });
      // when every initialization stuff is done, we set this.parentReady true
      // this is propagated from <main-component :child-ready="parentReady">
      // (in index.html) to App.vue and trigs a call to App's init function
      this.parentReady = true;

      // use this for magnetometer ? see :
      // https://blog.phonegap.com/migrating-from-the-cordova-device-orientation-plugin-8442b869e6cc
      // doesn't seem to work without it ...
    },
  },
});

document.addEventListener('deviceready', main.init);