#
# This file is licensed under the MIT license, the text of which can be found in the README file in
#     this repository, or online at <https://opensource.org/licenses/MIT>.
# Copyright (c) 2017 Sean Moss
#

# This code is derived from these two places:
# http://machinelearningmastery.com/machine-learning-in-python-step-by-step/
# http://scikit-learn.org/stable/auto_examples/classification/plot_classifier_comparison.html

import pandas
import numpy as np
from pandas.plotting import scatter_matrix
import matplotlib.pyplot as plt
plt.style.use('ggplot')
from matplotlib.colors import ListedColormap
from sklearn import model_selection
from sklearn.metrics import classification_report
from sklearn.metrics import confusion_matrix
from sklearn.metrics import accuracy_score
#from sklearn.linear_model import LogisticRegression
#from sklearn.tree import DecisionTreeClassifier
from sklearn.neighbors import KNeighborsClassifier
#from sklearn.discriminant_analysis import LinearDiscriminantAnalysis
from sklearn.naive_bayes import GaussianNB
from sklearn.svm import SVC


# Load in the dataset (hello world iris dataset for machine learning)
variable_names = [ 'sepal-length', 'sepal-width', 'petal-length', 'petal-width', 'class' ]
iris_data = pandas.read_csv('iris.data', names=variable_names)

# Summarize the data (shape, first few lines, ...)
print("Data Shape:", iris_data.shape)
print("\nData Example:\n", iris_data.head(5))
print("\nData Description:\n", iris_data.describe())
print("\nData Distribution:\n", iris_data.groupby('class').size())

# Generate a graphical representation of the feature distribution
print("\nShowing Feature Distribution...")
iris_data.plot(kind='box', subplots=True, layout=(2,2), sharex=False, sharey=False)
plt.show()

# Generate a histogram of the data
print("Showing Feature Histogram...")
iris_data.hist()
plt.show()

# Generate multivariate plots (allows visual inspection of classification grouping)
print("Showing Multivariate Scatter Plots...")
scatter_matrix(iris_data)
plt.show()

# Split the input data into a learning set (80%) and validation set (20%)
irisdata = iris_data.values
features = irisdata[:, 0:4]
classes = irisdata[:, 4]
classes_unique = {value: key for key, value in enumerate(np.unique(classes))}
iclasses = [classes_unique[classif] for classif in classes]
validation_size = 0.2
seed = 7
f_train, f_valid, c_train, c_valid = model_selection.train_test_split(
    features, classes, test_size=validation_size, random_state=seed
)

# Check accuracy of KNN, gNB, and SVM algorithm
models = [
    ('KNN', KNeighborsClassifier()),
    ('gNB', GaussianNB()),
    ('SVM', SVC())
]
results = [ None, None, None ]
names = [model[0] for model in models]
print('Calculating Algorithm Accuracies...')
for i, (name, model) in enumerate(models):
    kfold = model_selection.KFold(n_splits=10, random_state=seed)
    cvres = model_selection.cross_val_score(model, f_train, c_train, cv=kfold, scoring='accuracy')
    results[i] = cvres
    print('\t%s: %f (%f)' % (name, cvres.mean(), cvres.std()))

# Plot Algorithm Accuracies
print('Plotting algorithm accuracies...')
fig = plt.figure()
fig.suptitle('Algorithm Comparison')
ax = fig.add_subplot(111)
plt.boxplot(results)
ax.set_xticklabels(names)
plt.show()

# Make predictions using the validation data and the SVM algorithm
for (_, model) in models:
    model.fit(f_train, c_train)
svm = models[2][1]
svc_pred = svm.predict(f_valid)
print('\nPerforming Predicions...')
print('Accuracy Score:', accuracy_score(c_valid, svc_pred))
print('Confusion Matrix:\n', confusion_matrix(c_valid, svc_pred))
print('Classification Report:\n', classification_report(c_valid, svc_pred), '\n')


# Plot the solution maps to the various algorithms
print('Plotting algorithm solutions...')
splitdata = [
    (irisdata[:, 0], irisdata[:, 1]),
    (irisdata[:, 2], irisdata[:, 3])
]
cm_dim = ListedColormap(['#AA0000', '#00AA00', '#0000AA'])
cm_bright = ListedColormap(['#FF0000', '#00FF00', '#0000FF'])

# Create the subplot ranges and mesh grid
sxrng = (splitdata[0][0].min() - 0.5, splitdata[0][0].max() + 0.5)
syrng = (splitdata[0][1].min() - 0.5, splitdata[0][1].max() + 0.5)
pxrng = (splitdata[1][0].min() - 0.5, splitdata[1][0].max() + 0.5)
pyrng = (splitdata[1][1].min() - 0.5, splitdata[1][1].max() + 0.5)
sxx, syy = np.meshgrid(np.arange(sxrng[0], sxrng[1], 0.02), np.arange(syrng[0], syrng[1], 0.02))
spred = np.c_[sxx.ravel(), syy.ravel()]
pxx, pyy = np.meshgrid(np.arange(pxrng[0], pxrng[1], 0.02), np.arange(pyrng[0], pyrng[1], 0.02))
ppred = np.c_[pxx.ravel(), pyy.ravel()]

# Plot the raw data
fig, plots = plt.subplots(2, 4)
inplt = plots[0, 0]
inplt.set_title('Sepal Data')
inplt.scatter(splitdata[0][0], splitdata[0][1], c=iclasses, cmap=cm_bright)
inplt = plots[1, 0]
inplt.set_title('Petal Data')
inplt.scatter(splitdata[1][0], splitdata[1][1], c=iclasses, cmap=cm_bright)

# Plot each model solution
for i, (name, model) in enumerate(models):
    slplot = plots[0, i+1]
    plplot = plots[1, i+1]
    slplot.set_title(name + ' Sepals')
    plplot.set_title(name + ' Petals')
    # Make the predictions
    model.fit(f_train[:, 0:2], c_train)
    smodelpred = model.predict(spred)
    smodelpred = np.array([classes_unique[classif] for classif in smodelpred])
    smodelpred = smodelpred.reshape(sxx.shape)
    sscore = model.score(f_valid[:, 0:2], c_valid)
    model.fit(f_train[:, 2:], c_train)
    pmodelpred = model.predict(ppred)
    pmodelpred = np.array([classes_unique[classif] for classif in pmodelpred])
    pmodelpred = pmodelpred.reshape(pxx.shape)
    pscore = model.score(f_valid[:, 2:], c_valid)
    # Plot the contour field
    slplot.contourf(sxx, syy, smodelpred, cmap=cm_dim, alpha=0.3)
    plplot.contourf(pxx, pyy, pmodelpred, cmap=cm_dim, alpha=0.3)
    # Overplot the data
    slplot.scatter(splitdata[0][0], splitdata[0][1], c=iclasses, cmap=cm_bright, alpha=0.8)
    plplot.scatter(splitdata[1][0], splitdata[1][1], c=iclasses, cmap=cm_bright, alpha=0.8)
    # Write the solution score
    tpos = (sxx.max() - 0.3, syy.min() + 0.2)
    ttext = ('%.3f' % (sscore)).lstrip('0')
    slplot.text(tpos[0], tpos[1], ttext, size=15, horizontalalignment='right')
    tpos = (pxx.max() - 0.3, pyy.min() + 0.2)
    ttext = ('%.3f' % (pscore)).lstrip('0')
    plplot.text(tpos[0], tpos[1], ttext, size=15, horizontalalignment='right')
plt.show()
