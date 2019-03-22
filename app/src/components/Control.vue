<template>
  <div id="control-container">
    <div id="play-btn-wrapper-wrapper">
    <div id="play-btn-wrapper">
      <div
        id="play-btn"
        :class="playing ? 'button stop' : 'button play'"
        v-on:click="onPlayStopClicked">
      </div>
    </div>    
    </div>
  </div>
</template>

<script>
  export default {
    computed: {
      playing: {
        get() { return this.$store.state.playing; },
        set(value) { this.$store.commit('updatePlaying', value); },
      }
    },
    methods: {
      onPlayStopClicked() {
        this.$store.commit('updatePlaying', !this.playing);
      },
    },
  };
</script>