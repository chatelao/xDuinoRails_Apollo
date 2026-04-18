# Goal

Top-level business goal

# Structure
- `LICENSE.md` : We like a dual license with AGPL 3.0 (Affero GNU).
- `CONTRIBUTE.md` : Rules to contribute to the project with the dual license concept.
- `CONCEPT.md`: The overall structure of the product, including Business Cases & Use Cases as well as the overall High-Level Architecture, etc.
- `architecture.puml` : Architecture image kept up to date (rendered to `architecture/architecture*.png`)
- `DESIGN.md`: The detailed design of the solution, including the architecture, used tech stack for development, production and testing, etc.
- `ROADMAP.md`: The list of accomplished and planned steps of the project, it should be group into Phases, Tasks and Subtasks if necessary. Checkboxes show the progress to be updated with every increment.
- `specification`: External Know-How as datasheet, standards, etc. Should be converted to Markdown if PDF, etc.
- `src`: The source code of the project
- `test`: All tools, configurations & test cases
- `build`: Only temporary place for compilation, may be cached by Github

# Documentation
- Keep ".md" documentation filenames uppercase snake

# HOWTO
- Keep `src/install.sh` to install all tools to build the application (test only tools, see below)

# Testing Locally & with Github Action Workflow
- Write CI/CD test independent as `test` script of the Github Action Workflow
- Use `test/install.sh` to install test tools.
- Use the Github Action Workflow to run the tests after commits.
- Run the CI/CD on every commit on every branch
- Add  summarizing to each testcase including a referrence to the undlerying specification.

# Add the following GitHub actions:
- Provide all firmware examples with every release as assets
- Generate the Doxygen documents during build and provide the with the release assets
- 
