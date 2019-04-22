# mnpiano
### A very simple MIDI controller.

**NOTE**: This is still in early development. Some features aren't complete yet, and some things may be broken or subject to change.

`mnpiano` is a simple way to output MIDI notes to any ALSA sequencer-compatible client. By itself, it will not make any sound. I recommend using a software synth such as [TiMidity++](http://timidity.sourceforge.net/) or [FluidSynth](http://www.fluidsynth.org/) if you want to make sound on your computer.

`mnpiano` will work with hardware MIDI devices over USB as well, so long as they're compatible with ALSA.

## Building

You need both the `curses` and `asound` libraries to build. If you don't have them, the install process will depend on your distro. After that, it should build simply with `make`. I haven't made a `make install` yet, maybe once this project matures a little bit I will. If you really want to install it, copy the binary to somewhere in your `PATH`.

## Usage

First, you need to know which client/port you want to connect to. Use `aconnect -o` to show all open ports on the system. If you're planning on using TiMidity++ as a software synth, you need to start it first: `timidity -iA` (or `timidity -iAD` to daemonize it). `aconnect` will show something like:
```
$ aconnect -o
client 14: 'Midi Through' [type=kernel]
    0 'Midi Through Port-0'
client 128: 'TiMidity' [type=user,pid=15249]
    0 'TiMidity port 0 '
    1 'TiMidity port 1 '
    2 'TiMidity port 2 '
    3 'TiMidity port 3 '
```
Then run `mnpiano`:
```
$ ./mnpiano 128:0
```
Use backspace or CTRL-C to exit.
