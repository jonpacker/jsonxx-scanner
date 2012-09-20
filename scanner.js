var util = require('util'),
    events = require('events'),
    jxx = require('../build/Release/jsonxx.node');

function JxxScanner() {
  this.writable = true;
  this.buffer = null;
  this.jxx = new jxx.scanner(this);
};

util.inherits(JxxScanner, events.EventEmitter);

JxxScanner.prototype.write = function(buffer, encoding) {
  if (typeof buffer === 'string') {
    this.jxx.scanStr(buffer, encoding);
  } else {
    this.jxx.scan(buffer);
  }
  return true;
};

JxxScanner.prototype.end = function(buffer, encoding) {
  if (buffer) {
    this.write(buffer, encoding);
  }

  this.emit('close');
  this.writable = false;
};
