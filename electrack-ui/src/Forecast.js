import React, { useState, useEffect } from 'react';
import { Line } from '@ant-design/charts';

const DemoLine = () => {
    const [data, setData] = useState([]);
    useEffect(() => {
        asyncFetch();
    }, []);
    const asyncFetch = () => {
        fetch("http://localhost:8888/forecast")
            .then((response) => response.json())
            .then((json) => setData(json))
            .catch((error) => {
            console.log('fetch data failed', error);
        });
    };
    var config = {
        data: data,
        padding: 'auto',
        xField: 'day',
        yField: 'value',
        xAxis: { tickCount: 5 },
    };
    return <Line {...config}/>;
};
export default DemoLine;
