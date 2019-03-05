<template>
  <div id="settings-container">
    <div id="settings-wrapper">
      <div> input port </div>
      <div>  <input type="number" v-model="inputPort" :class="validInputPort"> </div>

      <div> output port </div>
      <div> <input type="number" v-model="outputPort" :class="validOutputPort"> </div>

      <div> host IP </div>
      <div class="ip-container">
        <input type="number" class="ip" v-model="ip1" :class="validHostIp">
        .
        <input type="number" class="ip" v-model="ip2" :class="validHostIp">
        .
        <input type="number" class="ip" v-model="ip3" :class="validHostIp">
        .
        <input type="number" class="ip" v-model="ip4" :class="validHostIp">
      </div>

      <div> device identifier </div>
      <div> <input type="text" v-model="deviceIdentifier"> </div>
    </div>
  </div>
</template>

<script>
  export default {
    data() {
      return {
        validInputPort: '',
        validOutputPort: '',
        validHostIp: '',
      };
    },
    computed: {
      inputPort: {
        get() { return this.$store.state.oscConfig.inputPort },
        set(inputPort) { this.$store.dispatch('updateOscConfig', { inputPort }); },
      },
      outputPort: {
        get() { return this.$store.state.oscConfig.outputPort },
        set(outputPort) { this.$store.dispatch('updateOscConfig', { outputPort }); },
      },
      ip1: {
        get() { return this.$store.state.oscConfig.hostIP.split('.')[0]; },
        set(value) { this.updateHostIP(value, 0); },
      },
      ip2: {
        get() { return this.$store.state.oscConfig.hostIP.split('.')[1]; },
        set(value) { this.updateHostIP(value, 1); },
      },
      ip3: {
        get() { return this.$store.state.oscConfig.hostIP.split('.')[2]; },
        set(value) { this.updateHostIP(value, 2); },
      },
      ip4: {
        get() { return this.$store.state.oscConfig.hostIP.split('.')[3]; },
        set(value) { this.updateHostIP(value, 3); },
      },
      deviceIdentifier: {
        get() { return this.$store.state.oscConfig.deviceIdentifier },
        set(deviceIdentifier) { this.$store.dispatch('updateOscConfig', { deviceIdentifier }); },
      },
    },
    methods: {
      updateHostIP(value, index) {
        const ip = this.$store.state.oscConfig.hostIP;
        let ipa = [ 0, 0, 0, 0 ];

        if (ip !== null && ip !== undefined) {
          ipa = ip.split('.');

          if (ipa.length > index) {
            ipa[index] = value;
          }
        }

        this.$store.dispatch('updateOscConfig', {
          hostIP: `${ipa.join('.')}`
        });
      }
    },
  };
</script>