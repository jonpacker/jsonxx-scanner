var util = require('util'),
    events = require('events');

function JxxScanner() {
  this.writable = true;
  this.buffer = null;
}


