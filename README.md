# spatial (no longer maintained)
Spatial and computational geometry related algorithms

TODO:
1. double accuracy

Important note:
1. if large files is created in an intermediate commit and later deleted, but cannot be pushed
   - backup the whole repo
   - $ git filter-branch -f --index-filter 'git rm --cached --ignore-unmatch path_to_file'
   - push again will fail
   - $ git pull
   - resolve all the conflicts
   - push again
