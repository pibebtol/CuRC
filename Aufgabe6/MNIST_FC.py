import sys
import os
import time

import numpy as np
import theano
import theano.tensor as T
theano.config.openmp = True

import lasagne
from lasagne.nonlinearities import rectify, softmax, sigmoid, tanh
from lasagne import layers, objectives
from lasagne.updates import sgd

from Load_MNIST import load_dataset

X, Y, X_test, Y_test = load_dataset()

num_train = X.shape[0] #Anzahl der Trainingsbeispiele
num_test = X_test.shape[0] #Anzahl der Testbeispiele
num_channels = X.shape[1] #1 Bei MNIST
height = X.shape[2] #28 Bei MNIST
width = X.shape[3] #28 Bei MNIST

batch_size = 50 #Batchgroesse 
num_iterations = 100 #Anzahl Iterationen

fc1_size = 15 #Größe hidden layer
out_size = 10 # Anzahl Klassen

input_var = T.tensor4('inputs') #Eingabe ist 4D: N Trainingsbeispiele, 1 Kanal, Hoehe, Breite
target_var = T.ivector('targets') #Ausgabe ist 1D

l_input = layers.InputLayer(shape=(None, num_channels, height, width), input_var=input_var) #Eingabeschicht
l_fc1 = layers.DenseLayer(l_input, num_units=fc1_size, nonlinearity=sigmoid) #Hidden Layer
l_fc_out = layers.DenseLayer(l_fc1, num_units=out_size, nonlinearity=softmax) #Ausgabeschicht

output = layers.get_output(l_fc_out) #Ausgabe des Netzes nach softmax (Anzahlbeispiele x Klassen (10))
output_class = T.argmax(output, axis=1) #Ausgabe Vektor der wahrscheinlichsten Klassen

params = layers.get_all_params([l_fc_out], trainable=True) # Gewichte und Biases des Netzes sammeln

cost = objectives.categorical_crossentropy(output, target_var).mean() #Kostenfunktion zum Minimieren
acc = T.mean(T.eq(output_class, target_var), dtype=theano.config.floatX) #Accuracy des Ergebnis

update = sgd(cost, params, learning_rate=0.01) # Funktion fuer die Aenderungen der Parameter

print("Building functions")
train_fn = theano.function([input_var, target_var], [cost, acc], updates=update) #Trainingsfunktion
test_fn = theano.function([input_var, target_var], [cost, acc, output_class]) #Testfunktion

print("Training")
#Trainingsiteration
for i in range(num_iterations):
    avg_acc = 0
    avg_cost = 0
    
    avg_acc_test = 0
    avg_cost_test = 0
    
    count = 0
    #Ueber Trainingsdaten in Batchen iterieren und trainieren
    for batch_start in range(0, num_train, batch_size):
        batch_stop = min(batch_start + batch_size, num_train)
        c, a = train_fn(X[batch_start:batch_stop],Y[batch_start:batch_stop])
        avg_cost += c
        avg_acc += a
        count +=1
    
    avg_acc /= count
    avg_cost /= count

    count = 0
    #Ueber Testdaten in Batchen iterieren
    for batch_start in range(0, num_test, batch_size):
        batch_stop = min(batch_start + batch_size, num_test)
        c, a, cl = test_fn(X_test[batch_start:batch_stop],Y_test[batch_start:batch_stop])
        avg_cost_test += c
        avg_acc_test += a
        count +=1
        
    avg_acc_test /= count
    avg_cost_test /= count
    
    print("Iteration %d: Train cost %.2f Train accuracy: %.2f%% Test cost %.2f Test accuracy %.2f%%" % (i+1, avg_cost, avg_acc*100, avg_cost_test, avg_acc_test*100))