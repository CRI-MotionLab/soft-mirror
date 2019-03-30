<template>
  <div id="control-container">
    <div id="play-btn-wrapper-wrapper">
    <div id="play-btn-wrapper">

      <div
        id="play-btn"
        :class="playing ? 'button stop' : 'button play'"
        v-on:click="onPlayStopClicked">
      </div>

      <div>
        <select
          id="mode-selector"
          class="menu-selector"
          v-bind:disabled="playing"
          v-on:change="onModeChanged"
          v-model="mode">
          <option v-for="m in modes" :value="m.value"> {{ m.text }} </option>
        </select>
      </div>
      
    </div>    
    </div>
  </div>
</template>

<script>
  export default {
    data() {
      return {
        modes: [
          { value: "centroid", text: "gesture control" },
          { value: "auto", text: "automatic" },
        ],
      };
    },
    computed: {
      mode: {
        get() { return this.$store.state.mode },
        set(value) { this.$store.dispatch('updateMode', value); },
      },
      playing: {
        get() { return this.$store.state.playing; },
        set(value) { this.$store.commit('updatePlaying', value); },
      }
    },
    methods: {
      onModeChanged() {
        this.$store.commit('updateMode', this.mode);
      },
      onPlayStopClicked() {
        this.$store.commit('updatePlaying', !this.playing);
      },
    },
  };
</script>