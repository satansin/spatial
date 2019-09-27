# spatial
Spatial and computational geometry related algorithms

TODO:
1. double accuracy

Important note:
1. if large files is created in an intermediate commit and later deleted, but cannot be pushed
1). backup the whole repo
2). $ git filter-branch -f --index-filter 'git rm --cached --ignore-unmatch path_to_file'
3). push again will fail
4). $ git pull
5). resolve all the conflicts
6). push again
