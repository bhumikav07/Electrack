import React, { useState, useEffect } from 'react';
import { Line } from '@ant-design/charts';
const DemoLine = () => {
    const [data, setData] = useState([]);
    useEffect(() => {
        asyncFetch();
    }, []);
    // useEffect(() => {
    //     console.log(data);
    // }, [data]);
    const asyncFetch = () => {
        var sock = new WebSocket("ws://192.168.1.11:3000"); //replace this address with the one the node.js server prints out. keep port 3000

        sock.onopen = function (event) {
            //alert("Socket connected succesfully!!!")
            console.log("Connection Successful");

        };
        sock.onmessage = function (event) {
            console.log(event); //show received from server data in console of browser
            var values = event.data.split(":");
            var datadetails = {
                "voltage": values[0],
                "current": values[1],
                "power": values[2],
                "energy": values[3]
            }

            var time = new Date();
            var y = time.getHours() + ":" + time.getMinutes() + ":" + time.getSeconds();
            var x = [{
                "date": y,
                "value": values[0],
                "category": "Voltage"
            },
            {
                "date": y,
                "value": values[1],
                "category": "Current"
            },
            {
                "date": y,
                "value": values[2],
                "category": "Power"
            },
            {
                "date": y,
                "value": values[3],
                "category": "Energy"
            }]
            console.log(...x);
            setData(b =>([...b, ...x]));
        }
    };



    var config = {
        data: data,
        xField: 'date',
        yField: 'value',
        seriesField: 'category',
        yAxis: {
            label: {
                formatter: function formatter(v) {
                    return ''.concat(v).replace(/\d{1,3}(?=(\d{3})+$)/g, function (s) {
                        return ''.concat(s, ',');
                    });
                },
            },
        },
        color : [ '#1979C9' , '#D62A0D' , '#FAA219' ],
    };
    return <Line {...config}/>;
};
export default DemoLine;