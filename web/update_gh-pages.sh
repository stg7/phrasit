#!/bin/bash
rm -rf gh-pages

git clone -b gh-pages git@github.com:stg7/phrasit.git gh-pages

cp -R static gh-pages/
cp *.html gh-pages/
cp *.json gh-pages/

cd gh-pages
git add .
git commit -m "update of github.io phrasit page @ $(date)"
git push origin

