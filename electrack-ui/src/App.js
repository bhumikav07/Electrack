import React, { useEffect, useState } from "react";
import "./App.less";
import Realtimechart from "./RealTimeChart.js";
import Monthlyprediction from "./MonthlyPrediction.js";
import ForecastGraph from "./Forecast.js";
import { Layout, Menu } from "antd";
import { RiseOutlined, PieChartOutlined } from "@ant-design/icons";
import { Statistic, Card, Row, Col } from "antd";


const { Header, Content, Sider } = Layout;

const App = () => {
  const [nav, setNav] = useState("1");
  // useEffect(() => {
  //
  // }, []);
  return (
    <>
      <Layout style={{ height: "100%" }}>
        <Sider
          breakpoint="lg"
          collapsedWidth="0"
          onBreakpoint={(broken) => {
            console.log(broken);
          }}
          onCollapse={(collapsed, type) => {
            console.log(collapsed, type);
          }}
        >
          <div style={{ display: "flex", placeItems: "center", marginTop: 16 }}>
            <h2 style={{ color: "white", flexGrow: 1, textAlign: "center" }}>
              ⚡ Elec-Track
            </h2>
          </div>
          <Menu theme="dark" mode="inline" selectedKeys={[nav]}>
            <Menu.Item
              key="1"
              icon={<PieChartOutlined />}
              onClick={() => setNav("1")}
            >
              Live Data
            </Menu.Item>
            <Menu.Item
              key="2"
              icon={<RiseOutlined />}
              onClick={() => setNav("2")}
            >
              Monthly Prediction
            </Menu.Item>
          </Menu>
        </Sider>
        <Layout>
          <Header
            className="site-layout-sub-header-background"
            style={{ padding: 0 }}
          />
          <Content style={{ margin: 24 }}>
            {nav === "1" ? (
              <>
                {" "}
                <Row gutter={16}>
                <Col className="gutter-row" span={6}>
                    <Card>
                      <Statistic
                        title="Voltage"
                        value={230}
                        precision={2}
                        valueStyle={{ color: "#1979C9" }}
                        suffix="V"
                      />
                    </Card>
                  </Col>
                  <Col className="gutter-row" span={6}>
                    <Card>
                      <Statistic
                        title="Current"
                        value={1.306}
                        precision={2}
                        valueStyle={{ color: "#D62A0D" }}
                        suffix="A"
                      />
                    </Card>
                  </Col>
                  <Col className="gutter-row" span={6}>
                    <Card>
                      <Statistic
                        title="Power"
                        value={9.3}
                        precision={2}
                        valueStyle={{ color: "#FAA219" }}
                        suffix="Wh"
                      />
                    </Card>
                  </Col>
                  <Col className="gutter-row" span={6}>
                    <Card>
                      <Statistic
                        title="Energy"
                        value={16.83}
                        precision={2}
                        valueStyle={{ color: "#58D3A2" }}
                        suffix="Wh"
                      />
                    </Card>
                  </Col>
                </Row>
                <Row style={{ marginTop: 24 }}>
                  <Col span={24}>
                    <Realtimechart />
                  </Col>
                </Row>
              </>
            ) : (
              <></>
            )}

            {nav === "2" ? (
              <>
                {" "}
                <Row style={{ marginTop: 24,fontSize:20}}>
                  <div>
                  <h1>Previous Data Visualization</h1>
                  </div>
                </Row>
                <Row style={{ marginTop: 24 }}>
                  <Col span={24}>
                    <Monthlyprediction/>
                  </Col>
                </Row>
                <Row style={{paddingTop:40,fontSize:20}}>
                  <div>
                  <h1>Forecast data for the next 30 days</h1>
                  </div>
                </Row>
                <Row style={{ marginTop: 24}}>
                  <Col span={24}>
                    <ForecastGraph/>
                  </Col>
                </Row>
              </>
            ) : (
              <></>
            )}
          </Content>
        </Layout>
      </Layout>
    </>
  );
};

export default App;
