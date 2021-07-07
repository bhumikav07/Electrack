import csv
from datetime import datetime
import json
import numpy
import numpy as np
import pandas as pd
import tensorflow as tf
from keras.models import Sequential
from keras.layers import Dense, LSTM
import matplotlib.pyplot as plt
from sklearn.preprocessing import MinMaxScaler
from tensorflow.keras.models import Sequential
from tensorflow.keras.layers import Dense
from tensorflow.keras.layers import LSTM
from sklearn.metrics import mean_squared_error
from numpy import array
from tensorflow.python.ops.gen_dataset_ops import flat_map_dataset

#Read the csv file in the input folder

class Prediction:
    def __init__(self):
        self.data = pd.read_csv("input/Apartment_3.csv")
        # Extract the necessary fields
        for col in list(self.data):
            if col not in ['date_time', 'date', 'power']:
                self.data = self.data.drop(col, 1)
        # Saving the processed dataset
        self.data.to_csv('building_data.csv')
        # Setting the date_itime as the index
        self.df = self.data.set_index('date_time')
        # Creating a new datset file and parse the data
        self.df.to_csv('cleaned_data.csv')
        self.dataset = pd.read_csv('cleaned_data.csv',
                            parse_dates=True,
                            index_col='date_time')
        #Resampling the data into Day Format
        self.power_data = self.dataset.resample('D').sum()

        self.model = Sequential()

    def visualise_history(self):
        po_wer = self.power_data.to_dict()
        values = [{
            "date": str(k.to_pydatetime()),
            "power": v
        } for k, v in po_wer['power'].items()]
        return ('data/visualize.json')

    def create_dataset(self, power_dataset, time_step=1):
        dataX, dataY = [], []
        for i in range(len(power_dataset) - time_step - 1):
            a = power_dataset[i:(i + time_step), 0]  ### i=0, 0,1,2,3----20
            dataX.append(a)
            dataY.append(power_dataset[i + time_step, 0])
        return numpy.array(dataX), numpy.array(dataY)

    def init_predict(self):
        self.scaler = MinMaxScaler(feature_range=(0, 1))

        self.power_data = self.scaler.fit_transform(np.array(self.power_data).reshape(-1, 1))
        training_size = int(len(self.power_data) * 0.80)
        test_size = len(self.power_data) - training_size
        self.train_data = self.power_data[0:training_size, :]
        self.test_data = self.power_data[training_size:len(self.power_data), :1]

        # reshape into X=t,t+1,t+2,t+3 and Y=t+4
        time_step = 20
        X_train, y_train = self.create_dataset(self.train_data, time_step)
        X_test, ytest = self.create_dataset(self.test_data, time_step)
        X_train = X_train.reshape(X_train.shape[0], X_train.shape[1], 1)
        X_test = X_test.reshape(X_test.shape[0], X_test.shape[1], 1)

        # LSTM_Model():
        # model = Sequential()
        self.model.add(LSTM(50, return_sequences=True, input_shape=(20, 1)))
        self.model.add(LSTM(50, return_sequences=True))
        self.model.add(LSTM(50))
        self.model.add(Dense(1))
        self.model.compile(loss='mean_squared_error', optimizer='adam')

        #Epoch Run
        self.model.fit(X_train,
                y_train,
                validation_data=(X_test, ytest),
                epochs=500,
                batch_size=64,
                verbose=1)

        # Lets Do the prediction and check performance metrics
        self.train_predict = self.model.predict(X_train)
        self.test_predict = self.model.predict(X_test)

        # Transformback to original form
        self.train_predict = self.scaler.inverse_transform(self.train_predict)
        self.test_predict = self.scaler.inverse_transform(self.test_predict)

    def prediction(self):
        # Plotting
        # Shift train predictions for plotting
        self.init_predict();
        
        look_back = 20
        trainPredictPlot = numpy.empty_like(self.power_data)
        trainPredictPlot[:, :] = np.nan
        trainPredictPlot[look_back:len(self.train_predict) +
                        look_back, :] = self.train_predict

        # Shift test predictions for plotting
        testPredictPlot = numpy.empty_like(self.power_data)
        testPredictPlot[:, :] = numpy.nan
        testPredictPlot[len(self.train_predict) + (look_back * 2) + 1:len(self.power_data) -
                        1, :] = self.test_predict
        scaler_power_data = self.scaler.inverse_transform(self.power_data)

        print(scaler_power_data)
        print(trainPredictPlot)
        print(trainPredictPlot)

    #demonstrate prediction for next 30 days
    def forecast(self):
        self.init_predict();
        lst_output = []
        x_input = self.test_data[56:].reshape(1, -1)
        temp_input = list(x_input)
        temp_input = temp_input[0].tolist()
        n_steps = 20
        i = 0
        while (i < 30):
            if (len(temp_input) > 20):
                #print(temp_input)
                x_input = np.array(temp_input[1:])
                print("{} day input {}".format(i, x_input))
                x_input = x_input.reshape(1, -1)
                x_input = x_input.reshape((1, n_steps, 1))
                #print(x_input)
                yhat = self.model.predict(x_input, verbose=0)
                print("{} day output {}".format(i, yhat))
                temp_input.extend(yhat[0].tolist())
                temp_input = temp_input[1:]
                #print(temp_input)
                lst_output.extend(yhat.tolist())
                i = i + 1
            else:
                x_input = x_input.reshape((1, n_steps, 1))
                yhat = self.model.predict(x_input, verbose=0)
                print(yhat[0])
                temp_input.extend(yhat[0].tolist())
                print(len(temp_input))
                lst_output.extend(yhat.tolist())
                i = i + 1

        day_new = np.arange(1, 21)
        day_pred = np.arange(21, 51)
        prev_day = self.scaler.inverse_transform(self.power_data[358:])
        next_few = self.scaler.inverse_transform(lst_output)

        # Converting array of lists to list
        new_day = day_new.tolist()
        pred_day = day_pred.tolist()
        # Converting array of lists to list
        previous =  prev_day.tolist()
        nextday = next_few.tolist()

        # Rounding of the list items to 2 decimal
        flat_prev = []
        for item in previous:
            flat_prev.extend(item)
        flat_previous = [round(num,2)for num in flat_prev]

        # Rounding of the list items to 2 decimal
        flat_next= []
        for item in nextday:
            flat_next.extend(item)
        flat_nextday = [round(num,2)for num in flat_next]

        #Converting the list into dataframe inorder to convert them to json
        result1 = pd.DataFrame(zip(new_day,flat_previous),columns =['day', 'value'])
        result2 = pd.DataFrame(zip(pred_day,flat_nextday),columns =['day', 'value'])

        result_prev = result1.to_json(orient="records")
        result_previous = json.loads(result_prev)

        result_next = result2.to_json(orient="records")
        result_nextday = json.loads(result_next)
        

        forecast_data = {'previous':result_previous,'next':result_nextday}
        return forecast_data

        # plt.plot(day_new, prev_day)
        # plt.plot(day_pred, next_few)
        # plt.show()

