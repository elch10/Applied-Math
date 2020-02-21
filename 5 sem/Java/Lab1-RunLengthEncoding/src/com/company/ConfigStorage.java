package com.company;

import java.io.BufferedReader;
import java.io.FileReader;
import java.io.IOException;
import java.util.HashMap;
import java.util.logging.Level;

public class ConfigStorage {

    public enum ConfigProperty {
        INPUT_FILE,
        OUTPUT_FILE,
        LOG_FILE,
        MODE;
    }

    public static final String DELIMITER = "=";

    private boolean isOk;
    private HashMap<String, String> properties = new HashMap<>();

    public ConfigStorage(String configFileName) {
        isOk = true;

        try (FileReader fin = new FileReader(configFileName);
             BufferedReader in = new BufferedReader(fin)) {

            for(String line = in.readLine() ; line != null ; line = in.readLine()) {

                if (line.equals("")) {
                    continue; // Empty lines
                }

                String args[] = line.replaceAll("\\s+","").split(DELIMITER); // Remove whitespaces and split

                if (args.length != 2) {
                    Main.writeToLog(Level.SEVERE,"Every line need to " +
                            "contain key and value in following format: " +
                            "key" + DELIMITER + "value\nBut got: " + line);
                    setBadStatus();
                    return;
                }

                properties.put(args[0], args[1]);
            }
        } catch (IOException e) {
            Main.writeToLog(Level.SEVERE, "Something went wrong: " + e.getMessage());
            setBadStatus();
            return;
        }

        for (ConfigProperty property: ConfigProperty.values()) {
            if (!properties.containsKey(property.toString())) {
                Main.writeToLog(Level.SEVERE,"\"" + property.toString() +
                        "\" property was not found in config file");
                setBadStatus();
                break;
            }
        }
    }

    private void setBadStatus() {
        isOk = false;
    }

    public boolean isStatusOk() {
        return isOk;
    }

    public String getPropertyValue(String propertyName) {
        return properties.get(propertyName);
    }
}
