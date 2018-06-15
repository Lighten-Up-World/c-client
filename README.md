# ARM11 project
Written in C for Imperial College London's Summer term project.

1. _Emulator:_ Simulates the execution of an ARM binary file on a Raspberry Pi
2. _Assembler:_ translates an ARM assembly source file into a
binary file that can subsequently be executed by the emulator
3. _LED:_ ARM assembly program that flashes an LED on a provided Raspberry Pi
4. _Extension "Lighten up World":_ Our general-purpose data visualisation project. Designed to represent data from a variety of sources.

## Emulator/Assembler

See `src` directory.

- `emulate.c` contains the main functionality for the emulator.
- `assemble.c` contains the main functionality for the assembler.

You can compile emulate using `make emulate`, and assemble using `make assemble`.


## Tests

- See `test` directory for the ruby test suite.
- See `src/unit_tests` directory for the unit tests.

Run `make` to compile emulate/assemble. Then this will run the unit_test and the ruby tests. Ensure submodules are imported using `git submodule init` and `git submodule update`

## Extension

- See `extension` directory for the extension code.

**Prerequistes:** If you're using Linux, first get the dependencies you need
(Mac users skip this step):

    apt-get install mesa-common-dev freeglut3-dev

Run `make server` inside extension/src folder to run the opengl viewer alone
Ensure submodules are imported using `git submodule init` and `git submodule update`

Fun commands to try:
-  `make conway`
-  `make raverplaid`
-  `make lavalamp`
-  `make simulation` for a heatmap of the world.
-  `make simulation MODE=windspeed` for a windspeed data.

To close the simulation close the window then ctrl-c the terminal.

## Git
[Git Branching Model](http://nvie.com/posts/a-successful-git-branching-model/)
### [Meaningful Messages](https://chris.beams.io/posts/git-commit/))
1. Separate subject from body with a blank line
2. Limit the subject line to 50 characters
3. Capitalize the subject line
4. Do not end the subject line with a period
5. Use the imperative mood in the subject line
6. Wrap the body at 72 characters
7. Use the body to explain what and why vs. how

### Branching Checklist
To create a new feature branchz
1. `git status` - Make sure you are in develop branch and all pre-requisite features have been merged. `git checkou develop` if not.
2. `git checkout -b myfeature develop`
To incorporate finished feature on develop.
1. `git status`
2. `git checkout develop`
3. `git merge --no-ff myfeature`
4. `git push origin develop`

### Quick checklist
1. `git status`
2. `git add -A`
3. `git commit -m "meaningful message"`
4. `git pull` before doing *any* pushing but after you have committed changes
5. `git push` changes.

## Documentation
See `doc` directory.

- `Checkpoint.tex` is the checkpoint report.
- `Report.tex` is the final report.

## Programs

## Authors

* **Tiger Cross**
* **Daniel Hails**
* **Will Burr**
* **Matt Malarkey**

## Acknowledgments

* Openpixelcontrol
* Unity
