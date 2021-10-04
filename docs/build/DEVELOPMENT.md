This page is for installing and using Slate on the native OS, as opposed to using Vagrant or Docker.

## Dependencies

Minimally, you will need the following:

* [Ruby](https://www.ruby-lang.org/en/) >= 2.5
* [Bundler](https://bundler.io/)
* [NodeJS](https://nodejs.org/en/)
* [Git](https://git-scm.com/)

Please note, only Linux and macOS are officially supported at this time. While slate should work on Windows, it is unsupported.

See below for installation instructions for different OSes / distros.

### Installing Dependencies on Linux

Install Ruby, NodeJS, and tools for compiling native ruby gems:

**On Ubuntu 18.04+**

```bash
sudo apt install ruby ruby-dev build-essential libffi-dev zlib1g-dev liblzma-dev nodejs patch
```

**On Fedora 31+**

```bash
sudo dnf install @development-tools redhat-rpm-config ruby ruby-devel libffi-devel zlib-devel xz-devel patch nodejs
```


Then, update RubyGems and install bundler:

```bash
sudo gem update --system
sudo gem install bundler
```

### Installing Dependencies on macOS

First, install [homebrew](https://brew.sh/), then install xcode command line tools:

```bash
xcode-select --install
```

Agree to the Xcode license:

```bash
sudo xcodebuild -license
```

Install nodejs runtime:

```bash
brew install node
```

Update RubyGems and install bundler:

```bash
gem update --system
gem install bundler
```

## Getting Set Up

1. Fork this repository on Github.
2. Clone *your forked repository* (not our original one) to your hard drive with `git clone https://github.com/YOURUSERNAME/slate.git`
3. `cd slate`
4. Install ruby gems for slate:

```shell
# either run this to run locally
bundle install
```

Note: if the above fails on installing nokogiri and using macOS see
[here](https://github.com/sparklemotion/nokogiri.org/blob/master/docs/tutorials/installing_nokogiri.md#macos)
for some helpful tips on things that might help.

## Running slate

You can run slate in two ways, either as a server process for development, or just build html files.

To do the first option, run:

```bash
bundle exec middleman server
```

and you should see your docs at http://localhost:4567. Whoa! That was fast!

The second option (building html files), run:

```bash
bundle exec middleman build
```

## What Now?

The next step is to [learn how to edit `source/index.md` to change the content of your docs](Markdown-Syntax). Once you're done, you might want to think about [deploying your docs](https://github.com/slatedocs/slate/wiki/Deploying-Slate).