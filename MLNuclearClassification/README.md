# MLNuclearClassification
Code for exploring classification of galactic nuclear regions using machine learning.

## Requirements
The generation of orbital data for analysis comes from n-body simulations done using [Luabound](https://github.com/mossseank/luabound), which is a wrapper around the [REBOUND](https://github.com/hannorein/rebound) gravity solver. You will need a recent C++ compiler (supporting C++14) to build Luabound and run the simulation.

The analysis code is written in Python 3.6, and will not work for other versions of Python. A small number of libraries (and the versions used, but not necessarily required) are needed as well, namely:

* `scipy (0.19.1)`
* `numpy (1.13.1)`
* `matplotlib (2.0.2)`
* `pandas (0.20.2)`
* `statsmodels (0.8.0)`
* `scikit-learn (0.18.2)`

An easy guide to set up a python environment with these can be found [here](http://machinelearningmastery.com/setup-python-environment-machine-learning-deep-learning-anaconda/).