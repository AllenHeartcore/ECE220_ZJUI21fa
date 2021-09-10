# Release
ECE 220 Release Repository

Welcome to ECE220! This repository contains all the lab assignments and machine problems that will be completed for the course. The following sections list some Git resources, explain the organization of this repository, how to retrieve/submit assignments, and how to view your grades.

More information about the course itself can be found at the [course website][1]


## Git Overview

Git is a distributed version control system to track and manage changes to files. This course uses Git for assignment distribution and submission.

### Recommendations

We strongly encourage you to follow these '[best practices][6]' for Git (or any version control systems). Although Git will be used in a somewhat limited scope in this class, it is beneficial to develop good habits early on.
* Learn about Git. A list of useful Git resources has been included below.
* Commit early and often. Having periodic checkpoints simplifies restoration in case of a catastrophic event and allows a full view of progress.
* Write useful commit messages. If anyone ever has to dig through your Git history, good commit messages will make their lives easier.
* Never check in broken code. Make sure that your code can always compile and run, even if you haven't implemented all the functionality.
* Test your code before committing. Please! Don't 'think' your code works after including some changes (even for something trivial as writing comments). Run some tests to ensure that it actually does.

### Organization and Workflow
You will be interacting with two repositories in this course:
1. Your personal repository under your NetID.
2. The release repository, `release`. This repository contains all lab assignments and machine problems for the course. Each individual assignment will be released under master branch at an appropriate time. All branches can be viewed through Github by selecting on the drop-down label `Branch: master`.

Before the start of every assignment, the course staff will push assignments to the `release` repository. You will have to merge the newly pushed assignments into your own repository. Once you have completed working on the assignments, you will commit and push your submission. The course staff will grade your lastest submission before the late deadline and post your grades to a `grade` branch in your own repository.

To ensure that this workflow will work properly please follow the instructions below.


## Git Resources

This course uses Git and Github as the primary means of distributing materials.
If you are unfamiliar with git, please take a look at the links below to learn
more about Git and its workflow:
* [Interactive Git tutorial][2]
* [Git cheatsheet][3]
* [Git transport commands and workflow][4]
* [Pro Git online book][5]

The above list is only a very small fraction of the Git resources that are
available online. We encourage you to research other resources as well.


## Course Setup

### Create Repository

To create a repository for the class, first login at UIUC's Github at https://github-dev.cs.illinois.edu.
Next, visit https://edu.cs.illinois.edu/create-ghe-repo/ece220-fa20-zjui/ to create a repository for this class. 
Now, if you navigate to https://github-dev.cs.illinois.edu/ece220-fa20-zjui, you will see a repository under your NetID. 
Click on your NetID and you will be shown a page that shows the contents of your repository. 
Initially, this repository will be empty but don't worry about that because it will be populated soon.

### Clone Repository

On the upper-left side of the page, there is a button **Clone or download**.
Click on the button and make sure it says **Clone with HTTPS**. If not, click on
the **Use HTTPS** text. Copy the link shown in the window which should be of the
form <https://github-dev.cs.illinois.edu/ece220-fa20-zjui/NetID.git>. In a terminal, navigate to the location where you would like to keep your files for ECE220. Type and paste the copied URL:

```
git clone https://github-dev.cs.illinois.edu/ece220-fa20-zjui/NetID.git
```

This will clone the remote repository to your computer where you will be able to retrieve/submit/work on assignments. 

### Add `Release` Remote

To retrieve assignments from the `release` repository you have to add it as a remote:

```
cd NetID
git remote add release https://github-dev.cs.illinois.edu/ece220-fa20-zjui/_release.git
```

Check that the remote was added:

```
git remote -v
```
You should see four lines of output, two for origin and two for release:

```
origin  https://github-dev.cs.illinois.edu/ece220-fa20-zjui/NetID.git (fetch)
origin  https://github-dev.cs.illinois.edu/ece220-fa20-zjui/NetID.git (push)
release https://github-dev.cs.illinois.edu/ece220-fa20-zjui/_release.git (fetch)
release https://github-dev.cs.illinois.edu/ece220-fa20-zjui/_release.git (push)
```

### Configure Repository

**It is important that you configure your repository correctly! If the following configuration parameters are not set, the auto-grader may not pull the correct versions of your submission and you may receive a lower score on your assignment. Regardless of which device you choose to work on your assignments (lab and/or personal machines), please set the configuration parameters described below.**

Set your name and email for your class repository:

```
git config user.name "Your Name"
git config user.email "NetID@illinois.edu"
```

Verify that your name and email have been correctly set:

```
git config --local --list
```


## Retrieve assignments

To retrieve (or update) released assignments, run:

```
git fetch release
git merge release/master -m "<some comment>"
git push origin master
``` 

where `<some comment>` is a comment of your choice. The last command pushes the
newly merged files to your remote repository. If something ever happens to your
repository and you need to go back in time, you will be able to revert your
repository to when you first retrieved an assignment.


## Submit assignments

To submit (or update) completed assignments, familiarize yourself with the
process below.

There are a couple of states that files may be in as you are working on your
assignment:
* unmodified tracked - files that are part of the local repository and have
  not had any changes made to them.
* modified tracked - files that are part of the local repository but have
  been changed.
* untracked - files that originally were not part of the local repository but
  have been created and are part of the workspace now.
All the files that are pulled/merged into your local repository are already
tracked.

To see all modified tracked and untracked files in your Git workspace, run:

```
git status
```

To stage any modified tracked or untracked files/directories for a commit, run:

```
git add <file/directory> <file/directory> ...
```

To commit any staged modified tracked or untracked files/directories to your
local repository run:

```
git commit <-a> -m "<some comment>"
```

Rather than individually staging any modified tracked files, you can stage them
all at once using the `<-a>` option with the above command. Note that this will
not stage any untracked files. Which commands to use to stage files/directories
depends on the situation. Individually staging files with `git add` gives you
more control over which changes will be part of the next commit whereas using
the `<-a>` option reduces the amount of typing necessary to stage all modified
tracked files.

Finally, push the committed file/directories in your local repository to the
remote repository with:

```
git push origin master
```

Verify that your submission are visible in the remote repository through a web browser. If you can see your changes, then the auto-grader can as well.


## Grades

To avoid issues with having to pull code from the remote repository, a new
branch called 'grade' will be created for you once the first assignment has been
graded. You can view your grades through the web browser by navigating to 
<https://github-dev.cs.illinois.edu/ece220-fa20-zjui/NetID.git> (replacing NetID with
your own) and clicking on the button that says `Branch: master`. A dropdown with
all available branches will appear. Click on the `grade` option. This will
display a list of directories corresponding to each of your assignments. Inside
these directories, you will find a `grade.txt` file with your score.

## AutoGrader

To be added soon.


## Personal Devices and Git Merge Conflicts

You may choose to work on class assignments on your own personal device rather than on lab machines. However, you should always verify that your submission works on D-331 machines because the D-331 environment is used by the auto-grader. If you do decide to use your own machine, you will have to learn how to pull your own code from your class repository and how to resolve potential merge conflicts.

### Workflow

Suppose that you have cloned your repository on both your lab machine and your personal device. At some point, you may have made edits and pushed these edits from one of the machines and wish to work on the other machine. To do so, you will first need to pull the changes from your class repository:

```
git pull origin master
```

This will bring in the edits you made from the other machine unto the current machine.

### Merge Conflicts

If there were any changes in the repository on your current machine, then Git may tell you that there are merge conflicts. To see the merge conflicts, run:

```
git status
```

There will be a list of files under the heading 'Unmerged paths' for any merge conflicts. For each merge conflict, do the following:
1. Open the file in a text editor
2. Find the lines with conflicts. Git places markers around these lines. For example:
   ```
   <<<<<<< HEAD
   i += 1;
   =======
   i += 2;
   >>>>>>> origin/master
   ```
   You can see the changes in HEAD or base branch after <<<<<<< HEAD. ======= separates the remote changes your trying to pull from the local conflicting changes. >>>>>>> origin/master marks the end of the conflicting changes.
3. Decide which of the changes you want to keep, if any. In the above example, we can include both changes and replace the 6 lines above with:
   ```
   i += 3;
   ```
   Please note that you should remove the markers placed by git (<<<<<<< ..., =======, >>>>>>> ... ).
4. Each file may have multiple conflicts. Repeat steps 2 and 3 until you cannot find <<<<<<< in your file.


[1]: http://lumetta.web.engr.illinois.edu/220-F20
[2]: https://try.github.io/levels/1/challenges/1
[3]: https://jan-krueger.net/wordpress/wp-content/uploads/2007/09/git-cheat-sheet.pdf
[4]: https://www.patrickzahnd.ch/blog.html#gitflow
[5]: https://git-scm.com/book/en/v2
[6]: https://sethrobertson.github.io/GitBestPractices/
