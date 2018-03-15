import sys
from PyQt5.QtCore import Qt
from PyQt5.QtWidgets import QWidget, QPushButton, QApplication, QLabel, QLineEdit, QCheckBox, QMessageBox
from PyQt5.QtCore import QCoreApplication
from PyQt5.QtGui import QIcon

import os
import matplotlib.pyplot as plt
from matplotlib import rcParams

from math import fabs
import numpy as np

epsilon = np.finfo(float).eps
arrange_epsilon = 0.0000001

OPERATORS = {'+': (1, lambda x, y: x + y), '-': (1, lambda x, y: x - y),
             '*': (2, lambda x, y: x * y), '/': (2, lambda x, y: x / y),
             's': (3, lambda x, y: np.sin(x * y)), 'c': (3, lambda x, y: np.cos(x * y)),
             '^': (3, lambda x, y: np.power(x, y)), 'e': (3, lambda x, y: x * np.power(np.e, y)),
             'l': (3, lambda x, y: np.log(y) / np.log(x)), '%': (3, lambda x, y: x * (-y))}


def eval_(formula):
    def parse(formula_string):
        number = ''
        for s in formula_string:
            if s in '1234567890.':
                number += s
            elif number:
                yield float(number)
                number = ''
            if s in OPERATORS or s in "()":
                yield s
        if number:
            yield float(number)

    def shunting_yard(parsed_formula):
        stack = []
        for token in parsed_formula:
            if token in OPERATORS:
                while stack and stack[-1] != "(" and OPERATORS[token][0] <= OPERATORS[stack[-1]][0]:
                    yield stack.pop()
                stack.append(token)
            elif token == ")":
                while stack:
                    x = stack.pop()
                    if x == "(":
                        break
                    yield x
            elif token == "(":
                stack.append(token)
            else:
                yield token
        while stack:
            yield stack.pop()

    def calc(polish):
        stack = []
        for token in polish:
            if token in OPERATORS:
                y, x = stack.pop(), stack.pop()
                stack.append(OPERATORS[token][1](x, y))
            else:
                stack.append(token)
        return stack[0]

    return calc(shunting_yard(parse(formula)))

def calculate_array(formula, x):
    fx = str(formula)
    y = []
    for i in x:
        if (-i == epsilon):
            i = 0;
        if i >= 0:
            newfx = fx[:fx.find('x')] + format(i, 'f') + fx[fx.find('x') + 1:]
            while newfx.find('x') != -1:
                newfx = newfx[:newfx.find('x')] + format(i, 'f') + newfx[newfx.find('x') + 1:]
            y.append(eval_(newfx))
        else:
            newfx = fx[:fx.find('x')] + "(1 % " + format(fabs(i), 'f') + ")" + fx[fx.find('x') + 1:]
            while newfx.find('x') != -1:
                newfx = newfx[:newfx.find('x')] + "(1 % " + format(fabs(i), 'f') + ")" + newfx[newfx.find('x') + 1:]
            y.append(eval_(newfx))
    return y

class ParseFormulas(QWidget):

    def __init__(self):
        super().__init__()

        self.initUI()

    def initUI(self):      

        self.useMachineEpsilon = False;
        self.FormulaText = "";
        self.MinValue = 1;
        self.MaxValue = 10;
        self.GridValue = 0.00001;

        self.lbl = QLabel(self)
        self.lbl.move(60, 120)

        self.menuLabel = QLabel(self)
        self.menuLabel.move(10, 10)
        self.menuLabel.setText("Calculate equasion by method of hords:")

        self.equalsZero = QLabel(self)
        self.equalsZero.move(150, 63)
        self.equalsZero.setText(" = 0")

        self.gridLabel = QLabel(self)
        self.gridLabel.move(10, 33)
        self.gridLabel.setText("Minimum:                      Maximum:                      ")
        self.gridLabel.adjustSize() 

        qle = QLineEdit(self)
        qle.move(10, 60)

        MinInput = QLineEdit(self)
        MinInput.move(60, 30)
        MinInput.resize(50, 20)

        MaxInput = QLineEdit(self)
        MaxInput.move(175, 30)
        MaxInput.resize(50, 20)

        #GridInput = QLineEdit(self)
        #GridInput.move(260, 30)
        #GridInput.resize(50, 20)


        #UseEpsilon = QCheckBox('Machine Epsilon', self)
        #UseEpsilon.move(280, 60)
        #UseEpsilon.stateChanged.connect(self.changeTitle)

        MinInput.textChanged.connect(self.MinChanged)
        MaxInput.textChanged.connect(self.MaxChanged)
        #GridInput.textChanged.connect(self.GridChanged)

        qle.textChanged.connect(self.onChanged)

        self.x = np.arange(self.MinValue, self.MaxValue, self.GridValue)

        calc = QPushButton('Calculate', self)
        calc.setCheckable(True)
        calc.move(180, 58)

        calc.clicked.connect(self.calculateFormula)
         
        self.setGeometry(300, 300, 260, 100)
        self.setWindowTitle('Icon')
        self.setWindowIcon(QIcon('web.png'))
        self.setWindowTitle('Method of hords')
        self.show()    
        
    def onChanged(self, text):
        self.FormulaText = text
        self.lbl.setText(self.FormulaText)
        self.lbl.adjustSize() 
        self.lbl.setText(" ")

    def MinChanged(self, text):
        if text != "-" and text != "":
            self.MinValue = float(text)
            self.lbl.setText(str(self.MinValue))
            self.lbl.adjustSize() 
            self.lbl.setText(" ")

    def MaxChanged(self, text):
        if text != "-" and text != "":
            self.MaxValue = float(text)
            self.lbl.setText(str(self.MaxValue))
            self.lbl.adjustSize() 
            self.lbl.setText(" ")

    def GridChanged(self, text):
        if text != "-":
            self.GridValue = float(text)
            self.lbl.setText(str(self.MaxValue))
            self.lbl.adjustSize() 
            self.lbl.setText(" ")

    def changeTitle(self, state):
      
        if state == Qt.Checked:
            self.useMachineEpsilon = True;
        else:
            self.useMachineEpsilon = False;

    def calculateFormula(self, pressed):
        source = self.sender()

        '''
        if self.useMachineEpsilon:
            arrange_epsilon = epsilon
        else:
            arrange_epsilon = self.GridValue
        '''

        iterations = 0

        temp_min = self.MinValue;
        temp_max = self.MaxValue;

        '''
        answer = ""
        answerString = ""
        temp_func_value = 0
        middle_point = temp_min

        while (fabs(float(calculate_array(self.FormulaText, [middle_point])[0])) > arrange_epsilon):
            middle_point = (temp_max + temp_min) / 2
            temp_func_value = float(calculate_array(self.FormulaText, [middle_point])[0])
            temp_min_value = float(calculate_array(self.FormulaText, [temp_min])[0])
            temp_max_value = float(calculate_array(self.FormulaText, [temp_max])[0])
            if (fabs(temp_min_value) < arrange_epsilon):
                middle_point = temp_min
                break
            elif (fabs(temp_max_value) < arrange_epsilon):
                middle_point = temp_max
                break
            else:
                Fmid = temp_max_value * temp_min_value
                if (Fmid < 0): 
                    temp_min = middle_point
                    #answerString += str(temp_min_value) + " ; " + str(temp_max_value) + " ; " + str(middle_point) + ";\n"
                elif (Fmid > 0): 
                    temp_max = middle_point
                    #answerString += str(temp_min_value) + " ; " + str(temp_max_value) + " ; " + str(middle_point) + ";\n"
                else: break

            if (fabs(temp_max - temp_min) < arrange_epsilon): break
            iterations+=1

            #answerString += str(Fmid) + ";\n"
            answerString += "X: " + str(temp_min) + " ; " + str(middle_point) + " ; " + str(temp_max) + ";\n"
            answerString += "Y: " + str(temp_min_value) + " ; " + str(temp_func_value) + " ; " + str(temp_max_value) + ";\n"
            #answerString += "X: " + str(middle_point) + " ; Y: " + str(calculate_array(self.FormulaText, [middle_point])[0]) + " ; " + str(arrange_epsilon) + ";\n"
        
        #middle_point = float(middle_point)

        for i in range(iterations):
            self.lbl.setText(answerString)
            self.lbl.adjustSize()

        middle_point = (temp_min + temp_max) / 2

        while (fabs(middle_point - temp_max) > epsilon):
            tmp = middle_point
            if (float(calculate_array(self.FormulaText, [temp_min])[0]) - float(calculate_array(self.FormulaText, [temp_max])[0]) == 0):
                break
            middle_point = temp_max - float(calculate_array(self.FormulaText, [temp_max])[0]) * (temp_min - temp_max) / (float(calculate_array(self.FormulaText, [temp_min])[0]) - float(calculate_array(self.FormulaText, [temp_max])[0]))
            temp_min = temp_max
            temp_max = tmp
            iterations+=1
        '''

        root = self.method_chord(temp_min, temp_max, epsilon)

        if (root[0] <= self.MaxValue and root[0] >= self.MinValue):
            QMessageBox.question(self, 'Answer', "X = " + str(root[0]) + ", in " + str(root[1] + 1) + " iterations.", QMessageBox.Close)
        else:
            QMessageBox.question(self, 'Answer', "No roots in this area!", QMessageBox.Close)

    def method_chord(self, x_prev, x_curr, e):
        iterations = 0
        x_next = (x_prev + x_curr) / 2
        while (fabs(x_next - x_curr) > e):
            tmp = x_next
            if ((float(calculate_array(self.FormulaText, [x_prev])[0]) - float(calculate_array(self.FormulaText, [x_curr])[0])) == 0):
                break
            x_next = x_curr - float(calculate_array(self.FormulaText, [x_curr])[0]) * (x_prev - x_curr) / (float(calculate_array(self.FormulaText, [x_prev])[0]) - float(calculate_array(self.FormulaText, [x_curr])[0]))
            x_prev = x_curr
            x_curr = tmp
            iterations+=1
        return [x_next, iterations]

if __name__ == '__main__':

    app = QApplication(sys.argv)
    ex = ParseFormulas()
    sys.exit(app.exec_())