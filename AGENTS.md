# Structure
- Keep sources in “src”, exceptions are possible depending on special technologies

# Documentation
- Keep ".md" documentation filenames uppercase snake
- Create and maintain on every change a README.md, add the usual summary
- Create a LICENSE.md if missing with AGPL 3.0 (Affero GNU)
- Create and maintain on every change a “CONTRIBUTE.md”
- Keep all user documentation in the “docs” directory and all developer/contributor documentation in the "specs" directory.
- Create and maintain on every change a “HOW_TO_USE.md”, “USER_MANUAL.md”,
  “CORE_CONCEPTS.md”, “DEVELOPER_REFERENCE.md” and “TECHNICAL_DEBTS.md”
- Update the architecture image (`architecture/architecture*.png`) if the `*.puml` source file seems outdated or changed, even if it was previously updated manually.

# Code
Follow the most used file & code naming conventions in the project:
- If not yet provided use “snake case”
- All C++ source code files (.h, .cpp) must be commented using Doxygen style. This includes file-level comments, class/struct comments, and comments for all public members (methods, variables).
- Comment lines if magic numbers or complex operations are used
-Vertically align all **equals signs** (`=`) and **other important operators** for readability.

# Testing
Add a comment to each testcase
- Summarizing the goal of the testcase in a short sentence.
- Summarizing the steps executed in 1-2 lines with longer sentences.
- List the references the underlying specification / documentation to each test case.

# Add the following GitHub actions:
- Build and test the software after every push on every branch
- Provide all firmware examples with every release as assets
- Generate the Doxygen documents during build and provide the with the release assets

# Workflow
- When working on a task from `specs/LIGHT_AND_AUX_CONCEPT.MD`, always read the concept document first to understand the context.
- After completing a phase from the roadmap, update `specs/IMPLEMENTATION_STATUS.MD` to reflect the progress.
- When working on a sound-related task, always read `specs/SOUND_CONCEPT.MD` first.
- After completing a task from `specs/SOUND_ROADMAP.MD`, update the roadmap to mark the task as complete.
