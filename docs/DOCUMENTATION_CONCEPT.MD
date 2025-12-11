# Documentation System Concept

## Executive Summary

The goal is to professionalize the project documentation by generating high-quality PDF manuals and a browsable HTML static site, integrated directly into the build pipeline.

We propose using **Sphinx**, the industry-standard tool for Python documentation, which is also widely used for general software documentation due to its robust output capabilities (HTML, PDF, ePub).

While the user expressed openness to moving away from Markdown, we recommend a **hybrid approach** using the **MyST-Parser** plugin. This allows us to maintain the ease of writing in Markdown for the majority of content while leveraging Sphinx's powerful features and the option to use reStructuredText (rST) for complex layouts if necessary.

## Toolchain Selection

### 1. Documentation Generator: **Sphinx**
*   **Why:** Sphinx is unrivaled in generating cohesive, cross-referenced documentation suites. It supports "ReadTheDocs" style HTML and, critically, high-quality PDF output via LaTeX.
*   **Configuration:** Controlled via a `conf.py` file in the source directory.

### 2. Markdown Support: **MyST-Parser**
*   **Why:** Allows Sphinx to parse `.md` files natively. This enables us to keep the existing `docs/*.MD` files without an immediate, forced migration to `.rst`.
*   **Capabilities:** MyST extends Markdown with rST-like directives, allowing for features like cross-references, admonitions (notes/warnings), and tables which are standard Markdown weak points.

### 3. PDF Generation: **LaTeX (via TeXLive)**
*   **Why:** To satisfy the "high quality PDF" requirement. Sphinx generates LaTeX source code, which is then compiled by `pdflatex` or `xelatex` into a professional PDF with proper typesetting, table of contents, and indexing.
*   **Alternative:** `weasyprint` (HTML-to-PDF) is easier to set up but often produces inferior page layouts compared to LaTeX.

### 4. Theme: **ReadTheDocs Theme**
*   **Why:** Provides the familiar, responsive sidebar navigation and polished look requested by the user.

## Proposed Architecture

### Directory Structure
We will restructure the `docs/` folder to separate source material from build artifacts.

```text
/
├── docs/
│   ├── source/              # All source documentation
│   │   ├── conf.py          # Sphinx configuration
│   │   ├── index.rst        # Root of the documentation tree (Table of Contents)
│   │   ├── _static/         # Custom CSS/images
│   │   ├── images/          # Moved image files
│   │   ├── manuals/         # Folder for user manuals
│   │   │   ├── USER_MANUAL.md
│   │   │   └── GETTING_STARTED.md
│   │   ├── references/      # Folder for developer docs
│   │   │   ├── RCN-200.EN.md
│   │   │   └── ...
│   │   └── requirements.txt # Python dependencies (sphinx, myst-parser, etc.)
│   ├── build/               # Git-ignored directory for output
│   └── Makefile / make.bat  # Build scripts
```

### Markdown vs. reStructuredText (rST) Strategy
*   **Keep Markdown (.md)** for standard content files (`USER_MANUAL.md`, `RCN-*.md`).
*   **Use rST (.rst)** for the `index.rst` (root map) because the `toctree` directive is central to Sphinx structure and easier to manage in its native format.
*   **Transition Policy:** If a specific document requires complex tables or specific PDF formatting that MyST Markdown cannot handle gracefully, convert that single file to rST.

## Build Pipeline Integration (CI/CD)

We will modify `.github/workflows/build.yml` to include a documentation job.

### New Job: `docs`
This job will run on `ubuntu-latest` and perform the following steps:

1.  **Install Dependencies:**
    *   Python packages: `sphinx`, `sphinx-rtd-theme`, `myst-parser`.
    *   System packages: `latexmk`, `texlive-latex-recommended`, `texlive-latex-extra`, `texlive-fonts-recommended` (required for PDF build).
2.  **Build HTML:**
    *   Command: `sphinx-build -b html docs/source docs/build/html`
3.  **Build PDF:**
    *   Command: `sphinx-build -b latex docs/source docs/build/latex`
    *   Command: `make -C docs/build/latex all-pdf`
4.  **Upload Artifacts:**
    *   Upload `docs/build/html` and `docs/build/latex/*.pdf` as GitHub Actions artifacts.
5.  **Release (Optional):**
    *   If the trigger is a release tag, attach the generated PDF to the release assets.

### Integration with ReadTheDocs.org (SaaS)
While the CI pipeline described above is "self-hosted" and generates artifacts directly, the repository structure will be fully compatible with the managed [ReadTheDocs.org](https://readthedocs.org) service.
*   To use the managed service, we would simply add a `.readthedocs.yaml` configuration file and import the repository on their dashboard.
*   **Recommendation:** Start with the GitHub Actions pipeline to keep everything self-contained ("into the build pipeline" as requested), but ensure compatibility for future switch if desired.

## Migration Steps

1.  **Restructure:** Move existing files from `docs/` root to `docs/source/`. Update image links.
2.  **Configure:** Create `docs/source/conf.py` and `docs/source/index.rst`.
3.  **Index:** Add all existing MD files to the `index.rst` `toctree`.
4.  **Pipeline:** Update `.github/workflows/build.yml` with the PDF build steps.
5.  **Verify:** Run the build locally and in CI to check PDF quality.

## Recommendation on "Changing away from Markdown"
**Do not abandon Markdown entirely.** The modern MyST parser is robust. Converting all existing documentation to rST is a high-effort, low-reward task unless specific formatting issues arise. We will start with MyST and only use rST where strictly necessary (e.g., the root index).
