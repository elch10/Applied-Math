package ru.spbstu.pipeline;

import java.io.BufferedReader;
import java.io.FileReader;
import java.io.IOException;
import java.util.Arrays;
import java.util.HashMap;

public class ConfigStorage {
    private String[] allowedPropertyNames;

    public static final String PROPERTY_DELIMITER = "=";
    public static final String COMMENT_SYMBOL = "#";

    // Zero or one whitespace at each side of `=` OR whitespace
    private static final String REGEX_DELIMITER = "\\s?" + PROPERTY_DELIMITER + "\\s?";

    private boolean isOk;
    private HashMap<String, String> properties = new HashMap<>();

    public ConfigStorage(String configFileName, String[] allowedPropertyNames) {
        isOk = true;
        this.allowedPropertyNames = allowedPropertyNames;

        try (FileReader fin = new FileReader(configFileName);
             BufferedReader in = new BufferedReader(fin)) {

            for(String line = in.readLine() ; line != null ; line = in.readLine()) {

                line = line.replaceAll("\\s+", " "); // Remove extra whitespaces

                if (line.equals("") || line.startsWith(COMMENT_SYMBOL)) {
                    continue; // Empty lines or comments
                }

                if (!processLine(line)) {
                    break;
                }
            }
        } catch (IOException e) {
            setBadStatus("Something went wrong: " + e.getMessage());
        }
    }

    // Return true, if it processed current line
    protected boolean processLine(String line) {
        if (line.contains(PROPERTY_DELIMITER)) { // property
            String[] args = line.split(REGEX_DELIMITER);

            if (args.length != 2 ||
                    (allowedPropertyNames != null &&
                            !Arrays.asList(allowedPropertyNames).contains(args[0]))) {
                setBadStatus("Every line need to contain key and value in following format: " +
                        "key" + PROPERTY_DELIMITER + "value\nBut got: " + line +
                        "\nAnd allowed property names is: " + String.join(",", allowedPropertyNames));
                return false;
            }

            properties.put(args[0], args[1]);
            return true;
        }
        return false;
    }

    protected void setBadStatus(String message) {
        isOk = false;
        Main.writeToLog(message);
    }

    public boolean isStatusOk() {
        return isOk;
    }

    public String getPropertyValue(String propertyName) {
        return properties.get(propertyName);
    }
}
