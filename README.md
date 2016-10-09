ebs - evidence based scheduler
================================

ebs is a command-line tool inspired by Joel Spolsky's evidence based
scheduling.

Add a task with its estimated time to completion in minutes.
```
ebs add my-task 120
```

Start doing a task (and stop any on-going task).
```
ebs do my-task
```

What am I doing now?
```
ebs top
```

Filter by name and list tasks.
```
ebs list pretty-urgent
```

Mark tasks as completed.
```
ebs tick my-task
```

Predict the completion time. You can filter this too.
```
ebs predict
```

Take a break.
```
ebs add go-home-and-rest 123
ebs do go-home-and-rest
```


Installation
-------------
```bash
git clone https://github.com/ryutaroikeda/ebs
cd ebs
# This will install the binary to ~/bin/ebs and create ~/.ebs
# You can instead run make and do it yourself
make install
ebs help
```

ebs requires you to specify the path to its internal data.
```
ebs --path ~/.ebs do my-task
```

In bash, you can create an alias.
```bash
alias ebs='ebs --path ~/.ebs'
```


Simple expressions
------------------

Filter tasks with expressions. Expressions are in conjunctive normal form.

Syntax
```
expression := empty | disjunction ( slash disjunction )*
disjunction := literal ( comma literal )*
literal := negation string | string
string := [a-z0-9]*
empty :=
comma := ,
slash := /
negation := !
```


Task sheet
----------

The task sheet is a tab-separated-values file with columns for task name, task
status, estimated time in secods, and actual time in seconds.


Time sheet
----------

The time sheet is a tab-separated-values file with columns for the start time
of the task and the task name.


Calendar
--------

To do.
