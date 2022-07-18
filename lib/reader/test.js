// const Reader = require('./lib/reader/build/Release/reader.node');

var os = require('os')

var EventEmitter = require("events").EventEmitter,
    util = require("util");

const emitter = new EventEmitter();

var driverType = null;
function setDriverType(type) {
    driverType = type;
}

// lazy load the C++ binding
var binding = null;
function loadBinding() {
    if( !binding ) {
        binding = require('bindings')('../lib/reader/build/Release/reader.node');
    }
}

//This class is a wrapper for `binding.Reader` class
function Reader() {
    //Inherit from EventEmitter
    EventEmitter.call(this);

    loadBinding();

    /* We also want to inherit from `binding.Reader`, but unfortunately,
        it's not so easy for native Objects. For example, the
        following won't work since `new` keyword isn't used:

        `binding.Reader.apply(this, arguments);`

        So... we do this craziness instead...
    */
    var thisPlusArgs = new Array(arguments.length + 1);
    thisPlusArgs[0] = null;
    for(var i = 0; i < arguments.length; i++) {
        thisPlusArgs[i + 1] = arguments[i];
    }
    console.log(binding)
    this._raw = new (Function.prototype.bind.apply(binding.Reader,
        thisPlusArgs) )();

    /* Now we have `this._raw` Object from which we need to
        inherit.  So, one solution is to simply copy all
        prototype methods over to `this` and binding them to
        `this._raw`
    */
    for(var i in binding.Reader.prototype)
        this[i] = binding.Reader.prototype[i].bind(this._raw);

    /* We are now done inheriting from `binding.Reader` and EventEmitter.

        Now upon adding a new listener for "data" events, we start
        polling the Reader device using `read(...)`
        See `resume()` for more details. */
    this._paused = true;
    var self = this;
    self.on("newListener", function(eventName, listener) {
        if(eventName == "data")
            process.nextTick(self.resume.bind(self) );
    });
}
//Inherit prototype methods
util.inherits(Reader, EventEmitter);
//Don't inherit from `binding.Reader`; that's done above already!

Reader.prototype.test = function test() {
    this._closing = true;
    // this.removeAllListeners();
    this._raw.test();

    this._closed = true;
};

Reader.prototype.callEmit = function callEmit(e) {
    this._raw.callEmit(e);
};

try {
    const r = new Reader();
    // r.close();

    setTimeout(() => {
        // r.test()
    }, 1000)

    // r.callEmit(emitter.emit.bind(emitter));
} catch(e) {
    console.error(e)
}

exports.Reader = Reader;
