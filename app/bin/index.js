// some ideas found here :
// http://www.cheesecode.fr/creer-une-application-mobile-avec-vue-js-2-et-cordova/

// TODO : try vueify-next
// https://github.com/FreekVR/vueify (supposed to work with babel 7)

const fs = require('fs-extra');
const path = require('path');
const sass = require('node-sass');
const browserify = require('browserify');
const babelify = require('babelify');
const vueify = require('vueify');
const config = require('./config');

const cwd = process.cwd();

fs.ensureFileSync(config.cssOutput);
fs.ensureFileSync(config.appSrcOutput);

fs.copySync(config.htmlInput, config.htmlOutput);
fs.copySync(config.assetsInput, config.assetsOutput);

sass.render({
  file: config.sassInput,
  includePaths: config.sassIncludes,
  outFile: config.cssOutput,
}, function(err, res) {
  if (err !== null) {
    console.error(err);
  } else {
    fs.writeFileSync(config.cssOutput, res.css);
    browserify(config.appSrcInput)
      .transform('babelify')
      .transform('vueify')
      .bundle()
      .pipe(fs.createWriteStream(config.appSrcOutput));
  }
});
