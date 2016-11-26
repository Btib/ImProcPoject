package ImProc;

import java.awt.BorderLayout;
import java.awt.Dimension;
import java.awt.FlowLayout;
import java.awt.GridLayout;
import java.awt.Toolkit;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.awt.image.BufferedImage;
import java.io.File;
import java.io.IOException;

import javax.imageio.ImageIO;
import javax.swing.BoxLayout;
import javax.swing.ImageIcon;
import javax.swing.JButton;
import javax.swing.JComboBox;
import javax.swing.JFileChooser;
import javax.swing.JFrame;
import javax.swing.JLabel;
import javax.swing.JPanel;
import javax.swing.JScrollPane;
import javax.swing.UIManager;
import javax.swing.filechooser.FileNameExtensionFilter;

public class WindowBuilder implements ActionListener {
	
	private JButton openImageButton;
	private JFileChooser imageChooser;
	private FileNameExtensionFilter filter;
	private File file;
	private File file2;
	private BufferedImage image;
	private BufferedImage image2;
	private JPanel upper;
	private JPanel lower;
	private JFrame mainframe;
	private JLabel jLabel;
	private JLabel jLabel2;
	private ImageIcon imageIcon;
	private ImageIcon imageIcon2;
	private JScrollPane scrollPane;
	private JScrollPane scrollPane2;
	private JComboBox<String> combobox;
	private Dimension dim ;
	private String[] algorithms = {"Elsõ", "Második"};
	private boolean notfirst;
		
	public WindowBuilder(){
		setupWindow();
	}
	
    private void setupWindow(){    	
    	notfirst = false;
    	UIManager.put("FileChooser.cancelButtonText", "Mégse");
    	
    	mainframe = new JFrame("Képfel");
    	mainframe.setLayout(new BorderLayout());
    	dim = Toolkit.getDefaultToolkit().getScreenSize();
    	mainframe.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
    	
    	upper = new JPanel();
    	upper.setLayout(new FlowLayout());
    	lower = new JPanel();
    	lower.setLayout(new FlowLayout());
    	
    	openImageButton = new JButton("Kép megnyitása");
    	openImageButton.addActionListener(this);
    	
    	filter = new FileNameExtensionFilter("Képfájlok", "jpg", "png", "gif", "jpeg");
    	imageChooser = new JFileChooser();
    	imageChooser.setFileFilter(filter);
    	imageChooser.setAccessory(new ImagePreview(imageChooser));
    	
    	file = null;
    	image = null;
    	image2 = null;
    	
    	combobox = new JComboBox<String>(algorithms);
    	
    	jLabel = new JLabel();
    	jLabel2 = new JLabel();   
    	
    	upper.add(openImageButton);
    	upper.add(combobox);
    	mainframe.add(upper, BorderLayout.NORTH);
    	mainframe.setResizable(true);
    	mainframe.pack();
    	mainframe.setLocation(dim.width/2-mainframe.getSize().width/2, dim.height/2-mainframe.getSize().height/2);
    	mainframe.setVisible(true);
    }

	@Override
	public void actionPerformed(ActionEvent e) {
		if (e.getSource() == openImageButton){
			 int returnVal = imageChooser.showDialog(mainframe, "Megnyitás");
		        if (returnVal == JFileChooser.APPROVE_OPTION) {		
		        	if(notfirst){
			        	lower.remove(scrollPane);
			         	lower.remove(scrollPane2);
		        	}
		            file = imageChooser.getSelectedFile();		            
		            String filePath = file.getAbsolutePath();
		            try
		            {
		              image = ImageIO.read(file);
		            }
		            catch (Exception ex)
		            {
		              ex.printStackTrace();
		              System.exit(0);
		            }
		            imageIcon = new ImageIcon(image);
		            jLabel = new JLabel();
		            jLabel.setIcon(imageIcon);
		            scrollPane = new JScrollPane();
		            scrollPane.setViewportView(jLabel);
		            System.out.println(imageIcon.getIconHeight());
		            scrollPane.setPreferredSize(new Dimension(imageIcon.getIconWidth() < dim.width/2 ? imageIcon.getIconWidth() : dim.width/2 - 75, imageIcon.getIconHeight() < dim.height ? imageIcon.getIconHeight() : dim.height - 30));
		            lower.add(scrollPane);
		            
		            try {
						Process process = new ProcessBuilder("char_rec/x64/Debug/char_rec.exe","filePath",(String) combobox.getSelectedItem()).start();
						process.waitFor();
					} catch (IOException ex) {
						ex.printStackTrace();
					} catch (InterruptedException e1) {
						e1.printStackTrace();
					}

		            file2 = new File("Img/wall_of_text_by_zach205-d54tmj1.png");//imageChooser.getSelectedFile();
		            
		            try
		            {
		              image2 = ImageIO.read(file2);
		            }
		            catch (Exception ex)
		            {
		              ex.printStackTrace();
		              System.exit(0);
		            }
		            imageIcon2 = new ImageIcon(image2);
		            jLabel2 = new JLabel();
		            jLabel2.setIcon(imageIcon2);

		            scrollPane2 = new JScrollPane();
		            scrollPane2.setViewportView(jLabel2);
		            scrollPane2.setPreferredSize(new Dimension(imageIcon2.getIconWidth() < dim.width/2 ? imageIcon2.getIconWidth() : dim.width/2 - 75, imageIcon2.getIconHeight() < dim.height ? imageIcon2.getIconHeight() : dim.height - 30));
		            lower.add(scrollPane2);
		            mainframe.add(lower, BorderLayout.CENTER);
		            mainframe.pack();
		            mainframe.setExtendedState(JFrame.MAXIMIZED_BOTH); 
			        mainframe.setLocation(dim.width/2-mainframe.getSize().width/2, dim.height/2-mainframe.getSize().height/2 - 10);
			        notfirst = true;
		            mainframe.setVisible(true);
		        } 
		}
	}
}
