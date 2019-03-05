<template>
  <div id="control-container">
    <div id="play-btn-wrapper-wrapper">
    <div id="play-btn-wrapper">
      <div
        id="play-btn"
        :class="playing ? 'button play' : 'button stop'"
        v-on:click="onPlayStopClicked">
      </div>
    </div>    
    </div>
  </div>
</template>

<script>
  export default {
    data() {
      return {
        playing: true,
      };
    },
    methods: {
      onPlayStopClicked() {
        this.playing = !this.playing;
      },
    },
  };
</script>