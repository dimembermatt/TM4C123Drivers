# Version Control

Version control systems are very helpful when developing software and hardware
with teams. By using version control systems, users can see when and what
changes were introduced to the project. They can audit changes, test different
versions of the project (this is called _feature branches_), and rollback
breaking changes to the project! Finally, version control allows everyone to
standardize their version of the project so everyone is on the same page.

Our version control system of choice is [_Git_](https://git-scm.com/). There is
a Git [book](https://git-scm.com/book/en/v2) - I highly recommend reading this
if you want to learn the insides and outsides on how Git works and how you can
leverage it for maximum productivity.

Other than Git, we need a remote hosting server to store versions of our
software. We use [Github](https://github.com/), which is one of the biggest hubs
of free and open source software (FOSS) projects in the world.

## Installation

> Install Git on Arch

```bash
sudo pacman -S git # This will prompt for your password
git config --global user.name "Your Name"
git config --global user.email "your_email@gmail.com"
```

> Install Git on Ubuntu

```bash
sudo apt install git-all # This will prompt for your password
git config --global user.name "Your Name"
git config --global user.email "your_email@gmail.com"
```

This tutorial will install Git via command line on ArchLinux and Ubuntu. Windows users may
download [Git for Windows](https://git-scm.com/) that can be used from the
Command Prompt or Powershell, or use a Bash emulation shell such as [Git
BASH](https://gitforwindows.org/) (recommended). macOS users may use `homebrew`.

## Cloning a Repo

After you've installed Git and made an account on Github, the first thing you
can do is grab your own version of
[UTRASWare](https://github.com/ut-ras/UTRASWare), which is our custom software
library for Robotathon 2021. In particular, we'd like for you to create a new
repository from UTRASWare, using the template button. It'll prompt you to give a
name for it, and then it'll generate a new repository for you with the relevant
files.

![UTRASWare template button](./images/UTRASWare_new_template.png)
![UTRASWare template page](./images/UTRASWare_new_template2.png)

After copying the HTTPS URL, we can then clone the repository with the following
command:

> Cloning a Repo

```bash
git clone https://github.com/ut-ras/UTRASWare-Example.git
```

![Cloning UTRASWare](./images/UTRASWare_cloning.png)

Our output should look something like above. When we `clone` a repository, we
are essentially downloading the project into our current directory and setting
it up to be used with Git.

## Repository Structure

Let's look inside!

> Repository Structure

```bash
cd UTRASWare-Example # Change directory into the repo we just cloned.
ls -all      # Look inside and view details
```

![UTRASWare structure](./images/UTRASWare_structure.png)

Inside, we can see both folders and files. In particular, we see a `README.md`
file, a `.gitignore` file, and a `.git` folder.

A `README.md` file is the introduction to the repository. It describes what's in
the repository, what is the purpose of the repository, and instructions on how
to install and use it.

A `.gitignore` file tells Git what files and folders to _ignore_ when committing
changes to the repository. We'll talk more about this later.

The `.git` folder is how Git tracks and manages the project. Don't delete this
folder! 

Besides these, our software is organized into several folders:

- `docs`: This is where the developers of UTRASWare will publish our
  documentation. Github finds the files in here and builds a static website, the
  one you are reading right now!
- `examples`: This folder contains Keil and CCS example projects that show how
  to use the low level driver APIs provided in TM4C123Drivers, which is the core
  subset of drivers used for UTRASWare. Our higher level drivers are built on
  these, and anyone is welcome to use them for Robotathon!
- `inc`: This includes folder contains important files that are needed for building
  TM4C123Drivers and UTRASWare. The `RegDefs.h` file provides definitions for
  TM4C peripheral registers, and the other files are startup files that generate
  our interrupt vector table (IVT) (ECE students will learn more about this in
  EE 306, EE 319K, EE 460N, and EE 445L).
- `lib`: This library folder contains the TM4C123Drivers source code. They are
  grouped by peripheral, such as `lib/GPIO/` or `lib/Timer` or `lib/I2C`, and
  each folder contains a header and implementation file. The header files are
  particularly important in software design, since they provide the application
  programming interface (API) for programmers to use. This contains function and
  object definitions, prerequisites and warnings, and usage instructions.
- `rasexamples`, `rasinc`, and `raslib`: These folders are the UTRASWare higher
  level driver equivalents of `examples`, `inc`, and `lib`. In here, you'll find
  all you need to get your robot in working shape without resorting to write the
  higher level drivers yourself.
- `resources`: This last folder provides some extra documentation and notes for
  those who want to dive deeper on how this library works, how drivers were
  developed, how the TM4C works, and how to contribute to the project.

You'll find that the way we structured this repository is one of many different
ways; there's a lot of people out there, and they all have different ideas on
what is a good file structure. Nethertheless, we hope that you understand why we
organized it this way and that it helps simplify your project design.

## Making Changes to the Repository and Publishing It

Now that you've taken a look at the repository, the next natural thing is to
make your changes to it for your purposes!

This could be by creating a new folder called `src` (or source) and creating a
new CCS or Keil project. 

![Adding files to the project](./images/UTRASWare_new_files.png)

As can be seen here, I've added a new project called `rascar` (We'll show you
how to make this project in the [Creating Your First Project](#creating-your-first-project)
tutorial).

### Git Status

> Checking the Git Project

```bash
git status
```

Running `git status`, we can see that there is an untracked full of stuff.
`git status` tells us the current state of the repository. We'll use this
command a couple more times in this process to track where we're at.

### Git Add

The next step to do is to add the files to our Git project.

> Adding files to Git

```bash
git add rascar
```

![Git Add results](./images/UTRASWare_git_add.png)

We can see now with another `git status` call that we've added some new files to
the project. 

### Git Commit

Now that the files are added/staged to the project, we need to now commit them,
or tell Git that we want to perform a savestate at this point. By committing the
files, we generate a local savestate denoted by a hash that we can roll back to
at any time if we want.

> Committing files to Git

```bash
git commit -m "My commit message"
```

![Git Commit results](./images/UTRASWare_git_commit.png)

The added files were now committed, and we can see on another `git status` that
we're ahead of the Github remote server version by 1 commit. We can also use
`git log` to tell us the commit history (both local and remote). We can see that
hash `ad7e32d` belongs to the commit we just did.

### Git Push

Finally, the last step is to merge our changed code back to the remote codebase.
This way, other users of the repository can see our changes and pull them!

> Pushing files to Github

```bash
git push
```

![Git Push results](./images/UTRASWare_git_push.png)

We can see that after logging into Github with our username and password, we
pushed our local version of the repository with the remote server and
synchronized it. If we check the Github page for our repo, we can find our new
commit and changes!

![Github update](./images/UTRASWare_github_update.png)

## Final Notes

This is the end of the Git version control tutorial. We've only scratched the
surface of what Git and Github can do, and as you use the technology more,
you'll dicscover dozens of tricks to speed up your workflow or to make software
development more convenient.

A couple of things that weren't covered, but warrant a google search down the
road:

- Merge conflicts and how to resolve them
- Pulling the latest version of the repository after someone else has made a
  change 
- Rewriting history by removing personally identifiable information (PII) or
  passwords from a repository
- Adding git hooks to automate software testing
- Publishing your own website for free using Github Pages
- Creating Feature Branches for new drivers
- Auditing and reviewing other people's code using Pull Requests (PRs)
- And more...
