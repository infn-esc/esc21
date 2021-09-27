# How to contribute to this site

To modify or add items to these pages:

- Fork the ESC repository and clone it locally.

        git clone https://github.com/<your-username>/esc21

- Modify a file or add a new one in the directories already present, or create
  a new directory. Notice that each file needs to:
  - be formatted using markdown
  - have an `.md` extension
  - start with a YAML header like:

        ---
        title: Title of the exercise
        layout: main
        ---

- Modify the document structure `menu` in _config.yml to point
  to your newly introduced file. `name` needs to be the name of
  the directory containing your file. `label` is the name which will
  appear in the menu on the left.

- Verify your changes locally running `jekyll`. For example:

        cd esc21
        docker run --rm -it -p 4000:4000 -v $PWD:/srv/jekyll jekyll/jekyll jekyll serve

  The site should be available at <http://localhost:4000/esc21> and be
  automatically updated every time a file is changed.

- Push your changes to Github and do a pull request with your changes.
  You can preview your changes by going to
  `https://<github-username>.github.io/esc21/`.

- Once the pull request is merged, your changes will be visible at
  <https://infn-esc.github.io/esc21/>
