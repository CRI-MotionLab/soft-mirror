#ifndef _SOFTMIRROR_CONFIGWEBPAGE_H_
#define _SOFTMIRROR_CONFIGWEBPAGE_H_

static const char configWebPage[] PROGMEM = R"=====(
<!DOCTYPE html>
<html>
<head>
  <meta charset="utf8">
  <title></title>
  <style>
    html,body,div,p,ul,li,h1,h2,h3,h4,h5,h6{margin:0;padding:0;}
    ul{padding-left:15px;}
    body{font-family:Helvetica;color:#6fb98f;background-color:#2c7873;}
    #page-wrapper{text-align:center;}
    #canvas-wrapper{position:relative;width:100%;height:400px;margin-bottom:20px;}
    canvas{position:absolute;background:rgb(0,0,0);margin-left:-200px;}
    #overlay{
      position:absolute;display:inline-block;margin-left:-200px;pointer-events:none;
      width:400px;height:400px;background-size:100% 100% !important;opacity:0.2;
    }
    #help,#control{width:400px;text-align:left;word-wrap:break-word;}
    #help-control-wrapper{width:100%;text-align:center;}
    #help-control-wrapper>div{display:inline-block;width:400px;text-align:center;}
    #title,#control{padding:10px;}
    #clear-save{float:right;}
  </style>
</head>
<body>
  <div id="page-wrapper">
    <div id="title"><h1>Soft Mirror</h1><h2>coil map editor</h2></div> 
    <div id="canvas-wrapper">  
      <canvas id="coilMap" width="400" height="400"></canvas>
      <div id="overlay"></div>
    </div>
    <div id="help-control-wrapper"><div>
      <div id="help"><ul>
        <li> click on the background to create a new coil </li>
        <li> click on an existing coil to select and / or drag it </li>
        <li> type escape to unselect the selected coil </li>
        <li> type the selected coil's integer identifier to update it </li>
      </ul></div>
      <div id="control">
        <input type="file" id="fileBtn" name="file" />
        <div id="clear-save">
        <input type="button" id="clearBtn" value="clear" />
        <input type="button" id="saveBtn" value="save" />
        </div>
      </div>
    </div></div>
  </div>
  <script type="text/javascript">
  window.onload = function() {
    var maxDims = [ 400, 400 ];
    var cvs = document.querySelector('#coilMap');
    var ctx = cvs.getContext('2d');
    var o = document.querySelector('#overlay');
    var f = document.querySelector('#fileBtn');
    var c = document.querySelector('#clearBtn');
    var s = document.querySelector('#saveBtn');
    var points = [];
    var active = -1;
    var pressed = false;
    var currentNumber = '';

    function getMousePosition(e) {
      var r = cvs.getBoundingClientRect();
      var x = Math.floor((e.clientX - r.left) / (r.right - r.left) * r.width);
      var y = Math.floor((e.clientY - r.top) / (r.bottom - r.top) * r.height);
      return [ x, y ];
    }

    function distance(pt1, pt2) {
      return Math.sqrt(Math.pow(pt1[0] - pt2[0], 2) + Math.pow(pt1[1] - pt2[1], 2));
    }

    var connection = new WebSocket('ws://'+location.hostname+':81/', ['arduino']);
    connection.onopen = function() {
      // connection.send('Connect '/* + new Date()*/);
      connection.send('Connected');
      connection.send('getcoilmap\n');
    };
    connection.onerror = function(error) { console.log('WebSocket Error ', error); };
    connection.onmessage = function(e) {
      var args = e.data.split('\n');
      console.log(args);
      if (args.length >= 6 && args[0] === 'coilmap' && args[1] === 'maxdims') {
        dims = [ parseInt(args[2]), parseInt(args[3]) ];
        var scaleDims = dims[0] !== maxDims[0] || dims[1] !== maxDims[1];
        points = [];
        if (args[4] === 'startmap') {
          var i = 5;
          while (args[i] !== 'endmap') {
            var coords = [ parseInt(args[i]), parseInt(args[i + 1]) ];
            if (scaleDims) {
              for (let j = 0; j < 2; j++) {
                coords[j] = Math.floor(coords[j] * maxDims[j] / dims[j]);
              }
            }
            var point = [ coords[0], coords[1], parseInt(args[i + 2]) ];
            points.push(point);
            i += 3;
          }
        }
      }
    }

    f.addEventListener('change', function(e) {
      var fr = new FileReader();
      fr.onload = function () { o.style.background = `url(${fr.result})`; }
      fr.readAsDataURL(e.target.files[0]);
    }, false);

    c.addEventListener('click', function(e) {
      points = [];
      active = -1;
    });

    s.addEventListener('click', function(e) {
      var indexes = [];
      for (var i = 0; i < points.length; i++) {
        var id = points[i][2];
        if (indexes.indexOf(id) !== -1) {
          window.alert('warning : duplicate id ' + id);
          break;
        } else if (id > points.length) {
          window.alert('warning : id ' + id + ' out of bounds');
          break;
        } else {
          indexes.push(id);
        }
      }

      if (indexes.length === points.length) {
        var coilmap = `coilmap\nmaxdims\n${maxDims[0]}\n${maxDims[1]}\nstartmap\n`;
        for (let i = 0; i < points.length; i++) {
          coilmap += `${points[i][0]}\n${points[i][1]}\n${points[i][2]}\n`
        }
        coilmap += 'endmap\n';
        console.log(coilmap);
        connection.send(coilmap);
      }
    });

    cvs.addEventListener('mousedown', function(e) {
      pressed = true;
      active = -1;
      var coords = getMousePosition(e);
      for (var i = 0; i < points.length; i++) {
        if (distance(points[i], coords) < 5) {
          active = i;
          currentNumber = '';
          break;
        }
      }
    });

    cvs.addEventListener('mousemove', function(e) {
      var coords = getMousePosition(e);
      if (active !== -1 && pressed) {
        points[active][0] = coords[0];
        points[active][1] = coords[1];
      }
    });

    window.addEventListener('mouseup', function(e) {
      pressed = false;
      if (active !== -1) return;
      var coords = getMousePosition(e);
      if (coords[0] > 0 && coords[0] < maxDims[0] - 1 &&
          coords[1] > 0 && coords[1] < maxDims[1] - 1) {
        coords.push(points.length + 1);
        points.push(coords);
      }
    });

    window.addEventListener('keydown', function(e) {
      if (e.key === 'Backspace') { e.preventDefault(); }
    });

    window.addEventListener('keyup', function(e) {
      if (e.key === 'c') {
        points = [];
      } else if (e.key === 'Backspace' && active !== -1) {
        if (currentNumber !== '') {
          currentNumber = currentNumber.slice(0, -1);
        } else {
          points.splice(active, 1);
          active = -1;
        }
      } else if (Number(e.key) >= 0 && Number(e.key) <= 9) {
        if (active !== -1) {
          currentNumber += e.key;
        }
      } else if (e.key === 'Enter') {
        if (active !== -1 && currentNumber !== '') {
          points[active][2] = Number(currentNumber);
          currentNumber = '';
        }
      } else if (e.key === 'Escape') {
        active = -1;
      }
    });

    function drawPoints() {
      ctx.clearRect(0, 0, maxDims[0], maxDims[1]);
      ctx.font = '14px Helvetica';

      for (var i = 0; i < points.length; i++) {
        ctx.fillStyle = (i === active) ? '#d00' : '#ddd';
        ctx.beginPath();
        ctx.ellipse(points[i][0], points[i][1], 5, 5, 0, 0, 2 * Math.PI);
        ctx.fill();
      }

      if (active !== -1) {
        ctx.fillStyle = '#ddd';
        var nb = points[active][2];

        if (currentNumber !== '') {
          ctx.fillStyle = '#d00';
          nb = Number(currentNumber);
        }

        ctx.fillText('current coil id : ' + nb, 10, maxDims[1] - 10);
      } else {
        currentNumber = ''; // atomatically reset currentNumber when no coil is selected
      }
    }

    function draw() {
      drawPoints();
      window.requestAnimationFrame(draw);
    }
    window.requestAnimationFrame(draw);
  };
  </script>
</body>
</html>
)=====";

#endif /* _SOFTMIRROR_CONFIGWEBPAGE_H_ */

