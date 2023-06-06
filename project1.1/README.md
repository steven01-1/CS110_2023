### Command line instructions
You can also upload existing files from your computer using the instructions below.
```
Git global setup
git config --global user.name "YOURNAME"
git config --global user.email "YOURNAME@shanghaitech.edu.cn"
```
### Create a new repository
```
git clone  https://autolab.sist.shanghaitech.edu.cn/gitlab/cs110_23s_projects/p1.1_fubzh_ouyangkzh.git 
cd "WORKDIR"

git switch -c main
touch README.md
git add README.md
git commit -m "add README"
git push -u origin main
```
###  Push an existing folder
```
cd existing folder
git init --initial-branch=main
git remote add origin https://autolab.sist.shanghaitech.edu.cn/gitlab/cs110_23s_projects/p1.1_fubzh_ouyangkzh.git
git add .
git commit "m "Initial commit' 
git push -u origin main
```
### Push an existing Git repository
```
cd existing repo
git remote rename origin old-origin
git remote add origin https://autolab.sist.shanghaitech.edu.cn/gitlab/cs110 23s projects/p1.1_fubzh_ouyangkzh.git 
git push -u origin --all 
git push -u origin --tags
```
