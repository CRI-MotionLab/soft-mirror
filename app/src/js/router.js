import Vue from 'vue';
import VueRouter from 'vue-router';

import Control from '../components/Control.vue';
import Settings from '../components/Settings.vue';

export default new VueRouter({
  // 'mode': 'history',
  // scrollBehavior: () => ({ y: 0 }),
  'routes': [
    { path: '/control', component: Control, props: true },
    { path: '/settings', component: Settings },
    { path: '*', redirect: '/control' }, // redirect to control at startup
  ]
});