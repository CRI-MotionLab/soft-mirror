import Vue from 'vue/dist/vue.js';
import Vuex from 'vuex';

Vue.use(Vuex);

const errorHandler = (filename, e) => {
  let msg;

  switch (e.code) {
    case FileError.QUOTA_EXCEEDED_ERR:
      msg = 'Storage quota exceeded';
      break;
    case FileError.NOT_FOUND_ERR:
      msg = 'File not found';
      break;
    case FileError.SECURITY_ERR:
      msg = 'Security error';
      break;
    case FileError.INVALID_MODIFICATION_ERR:
      msg = 'Invalid modification';
      break;
    case FileError.INVALID_STATE_ERR:
      msg = 'Invalid state';
      break;
    default:
      msg = 'Unknown error';
      break;
  };

  console.log('Error (' + filename + '): ' + msg);
};

const settingsFilename = 'softmirror-settings.json';

const store = new Vuex.Store({
  state: {
    oscConfig: {
      inputPort: 8001,
      outputPort: 8000,
      hostIP: '192.168.1.1',
    },
    motionValues: {
      x: 0,
      y: 0,
      z: 0,
      alpha: 0,
      beta: 0,
      gamma: 0,
    },
    orientationValues: {
      alpha: 0,
      beta: 0,
      gamma: 0,
    },
    mode: 'centroid',
    playing: false,
  },
  // see https://stackoverflow.com/questions/44309627/vue-jsvuex-how-to-dispatch-from-a-mutation
  // (actually it is more "how to mutate from a dispatch" which is achieved here)
  mutations: {
    updateOscConfig(state, config) {
      state.oscConfig = Object.assign({}, state.oscConfig, config);
    },
    updateMotionValues(state, values) {
      state.motionValues = Object.assign({}, values);
    },
    updateOrientationValues(state, values) {
      state.orientationValues = Object.assign({}, values);
    },
    updateMode(state, value) {
      state.mode = value;
    },
    updatePlaying(state, value) {
      state.playing = value;
    },
  },
  actions: {
    updateMode({ commit }, mode) {
      commit('updateMode', mode);
      // do something
    },
    updateOscConfig({ dispatch, commit }, config) {
      commit('updateOscConfig', config);
      dispatch('persist');
    },
    // how to use the file plugin :
    // https://www.neontribe.co.uk/cordova-file-plugin-examples/
    ////////// retrieve from file
    retrieve({ state, dispatch }) {
      return new Promise((resolve, reject) => {
        const pathToFile = `${cordova.file.dataDirectory}${settingsFilename}`;

        window.resolveLocalFileSystemURL(pathToFile, (fileEntry) => {
          fileEntry.file((file) => {
            const reader = new FileReader();

            reader.onloadend = function(e) {
              // sanity checks
              const config = {};
              const fileConfig = JSON.parse(this.result).oscConfig;
              for (let key in state.oscConfig) {
                config[key] = fileConfig[key] || state.oscConfig[key];
              }
              dispatch('updateOscConfig', config);
              resolve();
            };

            reader.readAsText(file);
          }, (function(filename, e) {
            if (e.code === FileError.NOT_FOUND_ERR) {
              dispatch('persist');
            }
          }).bind(null, settingsFilename));
        }, (function(filename, e) {
          if (e.code === FileError.NOT_FOUND_ERR) {
            dispatch('persist');
          }
        }).bind(null, settingsFilename));
      });
    },
    ////////// persist to file
    persist({ state }) {
      const settings = {};
      Object.assign(settings, state);
      [ 'motionValues', 'orientationValues' ].forEach((item) => {
        delete settings[item];
      });
      const data = JSON.stringify(settings, null, '\t');

      window.resolveLocalFileSystemURL(cordova.file.dataDirectory, function(directoryEntry) {
        directoryEntry.getFile(settingsFilename, { create: true }, function(fileEntry) {
          fileEntry.createWriter(function(fileWriter) {
            fileWriter.onwriteend = function(e) {
              /* do sth on write end */
            };

            fileWriter.onerror = function(e) {
              /* do sth on write error */
            };

            var blob = new Blob([ data ], { type: 'text/plain' });
            fileWriter.write(blob);
          }, errorHandler.bind(null, settingsFilename));
        }, errorHandler.bind(null, settingsFilename));
      }, errorHandler.bind(null, settingsFilename));
    },
  },
  // see https://codepen.io/CodinCat/pen/PpNvYr
  // (allows to watch variables from App.vue)
  getters: {
    mode: state => () => state.mode,
    playing: state => () => state.playing,
    inputPort: state => () => state.oscConfig.inputPort,
    motionValues: state => () => state.motionValues,
    orientationValues: state => () => state.orientationValues,
  },
});

export default store;